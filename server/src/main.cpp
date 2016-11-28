#include <algorithm>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <SFML/Network.hpp>
#include <iostream>
#include <memory>
#include <string.h>
#include <thread>
#include <math.h>
#include "../../shared/serialization.h"

const int DEFAULT_PORT = 5001;
const std::string DB_ADDR = "tcp://bernardcosgriff.com:3306";
const std::string DB_USER = "teamaccess";
const std::string DB_PASS = "password";

void printUsage(const std::string &progname) {
    std::cout << std::endl;
    std::cout << progname << " - Run the loque game server." << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE: " << progname << " [options]" << std::endl;
    std::cout << "\t-port <int>" << std::endl;
    std::cout << "\t\tport for client connections (default is " << DEFAULT_PORT << ")" << std::endl;
    std::cout << "\t-h" << std::endl;
    std::cout << "\t\tdisplay this help message" << std::endl;
}

/* Helpers */ 

// Retrieves the classes a user is a member of. This will throw. 
void getClassIds(sql::Connection& dbconn,
                int userId,
                std::vector<int>& classIds) {
    std::string query = "SELECT classId FROM ClassAssociations WHERE userId = ?";
    std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
    pstmt->setInt(1, userId);
    std::unique_ptr<sql::ResultSet> qRes(pstmt->executeQuery());
    while (qRes->next()) {
        int classId = qRes->getInt(1);
        classIds.push_back(classId);
    }
}

void getEnabledLevelIds(sql::Connection& dbconn,
                        int classId,
                        std::vector<int>& levelIds) {
    std::string query = "SELECT levelId FROM LevelAssociations WHERE classId = ?";
    std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
    pstmt->setInt(1, classId);
    std::unique_ptr<sql::ResultSet> qRes(pstmt->executeQuery());
    while (qRes->next()) {
        int levelId = qRes->getInt(1);
        levelIds.push_back(levelId);
    }
}

/* Request handlers */ 

void handleLogin(sql::Connection& dbconn,
                 std::string username, std::string userpass,
                 LoginResult& loginres) {
    loginres.userType = DNE;
    try {
        std::string query = "SELECT userId, isAdmin FROM User WHERE username = ? AND password = ?";
        std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
        pstmt->setString(1, username);
        pstmt->setString(2, userpass);
        std::unique_ptr<sql::ResultSet> qRes(pstmt->executeQuery());
        if (qRes->next()) {
            int id = qRes->getInt(1);
            int isAdmin = qRes->getInt(2);
            loginres.userId = id;
            loginres.userType = isAdmin ? Admin : Student; 
        }
    } catch (sql::SQLException& e) {
        std::cerr << "ERROR: SQL Exception from handleLogin: " << e.what() << std::endl;
    }
}

void handleCreateAcc(sql::Connection& dbconn,
                     std::string username, std::string userpass, UserType type,
                     LoginResult& res) {
    res.userType = DNE;
    try {
        std::string query = "INSERT INTO User(username, password, isAdmin, levelsCompleted, totalScore, totalTime) VALUES (?, ?, ?, ?, ?, ?)";
        std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
        pstmt->setString(1, username);
        pstmt->setString(2, userpass);
        pstmt->setBoolean(3, type == Admin);
        pstmt->setInt(4, 0);
        pstmt->setInt(5, 0);
        pstmt->setInt(6, 0);
        pstmt->execute();
        
        // Retrieve the added ID.
        query = "SELECT LAST_INSERT_ID()";
        pstmt.reset(dbconn.prepareStatement(query));
        std::unique_ptr<sql::ResultSet> qRes(pstmt->executeQuery());
        if (qRes->next()) {
            res.userId = qRes->getInt(1);
            res.userType = type;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "ERROR: SQL Exception from handleCreateAcc: " << e.what() << std::endl;
    }
}

void handleAddClassroom(sql::Connection& dbconn,
                        int userId, int classId, // Need className not classID. ClassID will be assigned on insertion (auto increment).
                        ActionResult& res) {
    res.success = false;
    try {
        std::string query = "INSERT INTO ClassAssociations(userId, classId) VALUES(?, ?)";
        std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
        pstmt->setInt(1, userId);
        pstmt->setInt(2, classId);
        pstmt->execute();
        res.success = true;
    } catch (sql::SQLException& e) {
        std::cerr << "ERROR: SQL Exception from handleAddClassroom: " << e.what() << std::endl;
        res.reason = e.what(); // TODO: Set user-friendly error string. 
    }
}

void handleCreateClassroom(sql::Connection& dbconn,
                           int userId, const std::string& className,
                           ActionResult& res) {
    res.success = false;
    try {
        std::string query = "INSERT INTO Class(className) VALUES(?)";
        std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
        pstmt->setString(1 , className);
        pstmt->execute();

        query = "SELECT LAST_INSERT_ID()";
        pstmt.reset(dbconn.prepareStatement(query));
        std::unique_ptr<sql::ResultSet> qRes(pstmt->executeQuery());
        qRes->next();
        int classId = qRes->getInt(1);

        handleAddClassroom(dbconn, userId, classId, res); 
    } catch (sql::SQLException& e) {
        std::cerr << "ERROR: SQL Exception from handleCreateClassroom: " << e.what() << std::endl;
        res.reason = e.what(); // TODO: Set user-friendly error string. 
    }
}

void handlePostStats(sql::Connection& dbconn,
                     int userId, const GameStats& stats,
                     ActionResult& res) {
    res.success = false;

    // TODO: We want to execute these queries atomically. 
    try {
        std::string query = "UPDATE User SET totalTime = totalTime + ?, totalScore = totalScore + ? WHERE userId = ?";
        std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
        pstmt->setInt(1, stats.secToComplete);
        pstmt->setInt(2, stats.pointsScored);
        pstmt->setInt(3, userId);
        pstmt->execute();

        query = "SELECT levelHighScore, completionTime from ScoreInfo WHERE userId = ? AND levelId = ?";
        pstmt.reset(dbconn.prepareStatement(query));
        pstmt->setInt(1, userId);
        pstmt->setInt(2, stats.levelId);
        std::unique_ptr<sql::ResultSet> qRes(pstmt->executeQuery());

        if (qRes->next()) {
            int oldHighScore = qRes->getInt(1);
            int oldCompletionTime = qRes->getInt(2);
            if (stats.pointsScored > oldHighScore || stats.secToComplete < oldCompletionTime) {
                int highScore = std::max(stats.pointsScored, oldHighScore);
                int completionTime = std::min(stats.secToComplete, oldCompletionTime);

                query = "UPDATE ScoreInfo SET levelHighScore = ?, completionTime = ? WHERE userId = ? AND levelId = ?";
                pstmt.reset(dbconn.prepareStatement(query));
                pstmt->setInt(1, highScore);
                pstmt->setInt(2, completionTime);
                pstmt->setInt(3, userId);
                pstmt->setInt(4, stats.levelId);
                pstmt->execute();
            }
        } else {
            query = "INSERT INTO ScoreInfo(userId, levelId, levelHighScore, completionTime) VALUES(?, ?, ?, ?)";
            pstmt.reset(dbconn.prepareStatement(query));
            pstmt->setInt(1, userId);
            pstmt->setInt(2, stats.levelId);
            pstmt->setInt(3, stats.pointsScored);
            pstmt->setInt(4, stats.secToComplete);
            pstmt->execute();
        }
        res.success = true;
    } catch (sql::SQLException& e) {
        std::cerr << "ERROR: SQL Exception from handlePostStats: " << e.what() << std::endl;
        res.reason = "";        // TODO: set reason
    }
}

void handleGetUserStats(sql::Connection& dbconn,
                        int userId,
                        UserStats& stats) {
    try {
        std::string query = "SELECT username, totalScore, totalTime FROM User where userId = ?";
        std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
        pstmt->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> qRes(pstmt->executeQuery());
        qRes->next();
        stats.userId = userId; 
        stats.username = qRes->getString(1);
        stats.totalScore = qRes->getInt(2);
        stats.totalSecPlayed = qRes->getInt(3);

        query = "SELECT levelId, levelHighScore FROM ScoreInfo WHERE userId = ?";
        pstmt.reset(dbconn.prepareStatement(query));
        pstmt->setInt(1, userId);
        qRes.reset(pstmt->executeQuery());
        while (qRes->next()) {
            int levelId = qRes->getInt(1);
            int levelHighScore = qRes->getInt(2);
            stats.highScores[levelId] = levelHighScore; 
        }

        getClassIds(dbconn, userId, stats.classIds); 
    } catch (sql::SQLException& e) {
        std::cerr << "ERROR: SQL Exception from handleGetUserStats: " << e.what() << std::endl;
    }
}

void handleGetEnabledLevels(sql::Connection& dbconn,
                            int userId,
                            std::vector<int>& levelIds) {
    try {
        std::vector<int> classIds;
        getClassIds(dbconn, userId, classIds);
        for (auto classId : classIds) {
            getEnabledLevelIds(dbconn, classId, levelIds); 
        }
    } catch(sql::SQLException& e) {
        std::cerr << "ERROR: SQL Exception from handleGetEnabledLevels: " << e.what() << std::endl;
    }
}

void handleEnableLevel(sql::Connection& dbconn,
                       int userId, int classId, int levelId,
                       ActionResult& res) {
    res.success = false;
    try {
        std::string query = "INSERT INTO LevelAssociations(classId, levelId) VALUES(?, ?)";
        std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
        pstmt->setInt(1, classId);
        pstmt->setInt(2, levelId);
        pstmt->execute();
    } catch (sql::SQLException& e) {
        std::cerr << "ERROR: SQL Exception from handleEnableLevel: " << e.what() << std::endl;
        res.reason = "";        // TODO: set reason. 
    }
}

void handleDisableLevel(sql::Connection& dbconn,
                        int userId, int classId, int levelId,
                        ActionResult& res) {
    res.success = false;
    try {
        std::string query = "DELETE FROM LevelAssociations WHERE classId = ? AND levelId = ?";
        std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
        pstmt->setInt(1, classId);
        pstmt->setInt(2, levelId);
        pstmt->execute();
    } catch (sql::SQLException& e) {
        std::cerr << "ERROR: SQL Exception from handleDisableLevel: " << e.what() << std::endl;
        res.reason = "";        // TODO: set reason.
    }
}

void handleGetClassStats(sql::Connection& dbconn,
                         int userId, int classId,
                         ClassStats& stats) {
    try {
        std::string query = "SELECT userId FROM ClassAssociations WHERE classId = ?";
        std::unique_ptr<sql::PreparedStatement> pstmt(dbconn.prepareStatement(query));
        pstmt->setInt(1, classId);
        std::unique_ptr<sql::ResultSet> qRes(pstmt->executeQuery());
        while (qRes->next()) {
            int userId = qRes->getInt(1);
            UserStats userStats;
            stats.studentStats.push_back(userStats);
            handleGetUserStats(dbconn, userId, stats.studentStats.back()); 
        }
    } catch (sql::SQLException& e) {
        std::cerr << "ERROR: SQL Exception from handleGetClassStats: " << e.what() << std::endl; 
    }
}

void handleClient(std::unique_ptr<sf::TcpSocket> client,
                  std::unique_ptr<sql::Connection> dbconn) {
    sf::Packet reqPacket;
    auto status = client->receive(reqPacket);
    if (status != sf::Socket::Done) {
        std::cerr << "ERROR: Unable to receive request from " <<
                  client->getRemoteAddress().toString() <<
                  ". receive() returned status " << status << std::endl;
        return;
    }

    sf::Packet respPacket;
    ReqType rtype;
    reqPacket >> rtype;
    switch (rtype){
    case LOGIN:
    {
        std::string username, userpass;
        reqPacket >> username >> userpass;
        LoginResult res;
        handleLogin(*dbconn, username, userpass, res);
        respPacket << res;
        break;
    }
    case CREATE_ACC:
    {
        std::string username, userpass;
        UserType type;
        reqPacket >> username >> userpass >> type;
        LoginResult res;
        handleCreateAcc(*dbconn, username, userpass, type, res);
        respPacket << res;
        break;
    }
    case ADD_CLASS:
    {
        int userId, classId;
        reqPacket >> userId >> classId;
        ActionResult res;
        handleAddClassroom(*dbconn, userId, classId, res);
        respPacket << res;
        break;
    }
    case CREATE_CLASS:
    {
        int userId;
        std::string className;
        reqPacket >> userId >> className;
        ActionResult res;
        handleCreateClassroom(*dbconn, userId, className, res);
        respPacket << res;
        break;
    }
    case POST_STATS:
    {
        int userId;
        GameStats stats;
        respPacket >> userId >> stats;
        ActionResult res;
        handlePostStats(*dbconn, userId, stats, res);
        respPacket << res;
        return;
    }
    case GET_USER_STATS:
    {
        int userId;
        reqPacket >> userId;
        UserStats res;
        handleGetUserStats(*dbconn, userId, res);
        respPacket << res;
        break;
    }
    case GET_ENABLED_LEVELS:
    {
        int userId;
        reqPacket >> userId;
        std::vector<int> levelIds;
        handleGetEnabledLevels(*dbconn, userId, levelIds);
        respPacket << levelIds;
        break;
    }
    case ENABLE_LEVEL:
    {
        int userId, classId, lid;
        reqPacket >> userId >> classId >> lid;
        ActionResult res;
        handleEnableLevel(*dbconn, userId, classId, lid, res);
        respPacket >> res;
        break;
    }
    case DISABLE_LEVEL:
    {
        int userId, classId, lid;
        reqPacket >> userId >> classId >> lid;
        ActionResult res;
        handleDisableLevel(*dbconn, userId, classId, lid, res);
        respPacket >> res;
        break;
    }
    case GET_CLASS_STATS:
    {
        int userId, classId;
        reqPacket >> userId >> classId;
        ClassStats res;
        handleGetClassStats(*dbconn, userId, classId, res);
        respPacket >> res;
        break; 
    }
    default:
        std::cerr << "ERROR: Unrecognized request type " << static_cast<int>(rtype) << std::endl;
        return;
    }

    status = client->send(respPacket);
    if (status != sf::Socket::Done) {
        std::cerr << "ERROR: Unable to send response to " <<
            client->getRemoteAddress().toString() <<
            ". send() returned status " << status << std::endl;
    }
}

int main(int argc, char **argv) {
    int port = DEFAULT_PORT;
    if (argc > 1) {
        for (int i = 1; i < argc; i += 2) {
            if (strcmp(argv[i], "-port") == 0 && argc > i + 1) {
                port = std::stoi(argv[i + 1]);
            }
            if (strcmp(argv[i], "-h") == 0) {
                printUsage(std::string(argv[0]));
                return 0;
            }
        }
    }

    std::cout << "Starting Loque Server on port " << port << "." << std::endl;

    sql::Driver *driver = get_driver_instance();
    sf::TcpListener listener;
    int status = listener.listen(port);
    if (status != sf::Socket::Done) {
        std::cerr << "ERROR: Unable to listen on port " << port << "." <<
                  "listen() returned status " << status << "." << std::endl;
        return 1;
    }
    while (true) {
        std::unique_ptr<sf::TcpSocket> client(new sf::TcpSocket);
        int status = listener.accept(*client);
        if (status != sf::Socket::Done) {
            std::cerr << "ERROR: Unable to accept client connection." <<
                      "accept() returned status " << status << "." << std::endl;
            continue;
        }
        std::cout << "Accepted client connection with " <<
                  client->getRemoteAddress().toString() << std::endl;
        std::unique_ptr<sql::Connection> dbconn(driver->connect(DB_ADDR, DB_USER, DB_PASS));
        dbconn->setSchema("3505");
        std::thread worker(handleClient, std::move(client), std::move(dbconn));
        worker.detach();
    }
}
