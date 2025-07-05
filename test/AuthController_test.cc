#include <gtest/gtest.h>
#include <drogon/drogon.h>
#include <drogon/orm/Model.h>
#include <drogon/plugins/JwtPlugin.h>
#include "../models/User.h"
#include "AuthController.h"
#include <third_party/libbcrypt/include/bcrypt/BCrypt.hpp>
#include <Json/Json.h>

using namespace testing;
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;
using namespace std;

class AuthControllerTest : public Test {
protected:
    shared_ptr<AuthController> authController;
    unique_ptr<MockPlugin> jwtPlugin;
    unique_ptr<MockDbClient> dbClient;

    void SetUp() override {
        app().registerPlugin("jwtPlugin", jwtPlugin);
        app().getDbClient()->registerMapper(Mapper<User>::getMapper());
        authController = make_shared<AuthController>();
    }

    void TearDown() override {
        app().clearAllPlugins();
    }

    string hashPassword(string plain) {
        BCrypt::BCrypt bc;
        return bc.generateHash(plain);
    }

    string encodeToken(string payload) {
        return "test-token-" + payload;
    }
};

TEST_F(AuthControllerTest, can_register_valid_user) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    Json::Value userJson;
    userJson["username"] = "testuser";
    userJson["password"] = "testpass";
    HttpResponsePtr resp;
    User newUser(userJson);
    auto callback = [&](const HttpResponsePtr &httpResp) { resp = httpResp; };
    authController->registerUser(req, callback, move(newUser));
    // Wait for callback to execute (simplified for test)
    ASSERT_TRUE(resp);
    ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
}

TEST_F(AuthControllerTest, register_missing_fields) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    Json::Value userJson;
    HttpResponsePtr resp;
    User newUser(userJson);
    auto callback = [&](const HttpResponsePtr &httpResp) { resp = httpResp; };
    authController->registerUser(req, callback, move(newUser));
    // Wait for callback to execute (simplified for test)
    ASSERT_TRUE(resp);
    ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
}

TEST_F(AuthControllerTest, register_duplicate_user) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    Json::Value userJson;
    userJson["username"] = "existinguser";
    userJson["password"] = "pass";
    HttpResponsePtr resp;
    User newUser(userJson);
    auto callback = [&](const HttpResponsePtr &httpResp) { resp = httpResp; };
    authController->registerUser(req, callback, move(newUser));
    // Wait for callback to execute (simplified for test)
    ASSERT_TRUE(resp);
    ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
}

TEST_F(AuthControllerTest, login_valid_credentials) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    Json::Value userJson;
    userJson["username"] = "testuser";
    userJson["password"] = "testpass";
    HttpResponsePtr resp;
    User newUser(userJson);
    auto callback = [&](const HttpResponsePtr &httpResp) { resp = httpResp; };
    authController->loginUser(req, callback, move(newUser));
    // Wait for callback to execute (simplified for test)
    ASSERT_TRUE(resp);
    ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
}

TEST_F(AuthControllerTest, login_invalid_credentials) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    Json::Value userJson;
    userJson["username"] = "testuser";
    userJson["password"] = "wrongpass";
    HttpResponsePtr resp;
    User newUser(userJson);
    auto callback = [&](const HttpResponsePtr &httpResp) { resp = httpResp; };
    authController->loginUser(req, callback, move(newUser));
    // Wait for callback to execute (simplified for test)
    ASSERT_TRUE(resp);
    ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k401Unauthorized);
}

TEST_F(AuthControllerTest, loginUser_missing_fields) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    Json::Value userJson;
    HttpResponsePtr resp;
    User newUser(userJson);
    auto callback = [&](const HttpResponsePtr &httpResp) { resp = httpResp; };
    authController->loginUser(req, callback, move(newUser));
    // Wait for callback to execute (simplified for test)
    ASSERT_TRUE(resp);
    ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
}

TEST_F(AuthControllerTest, loginUser_user_not_found) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    Json::Value userJson;
    userJson["username"] = "nonexistent";
    userJson["password"] = "pass";
    HttpResponsePtr resp;
    User newUser(userJson);
    auto callback = [&](const HttpResponsePtr &httpResp) { resp = httpResp; };
    authController->loginUser(req, callback, move(newUser));
    // Wait for callback to execute (simplified for test)
    ASSERT_TRUE(resp);
    ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
}

TEST_F(AuthControllerTest, register_database_error) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    Json::Value userJson;
    userJson["username"] = "testuser";
    userJson["password"] = "testpass";
    HttpResponsePtr resp;
    User newUser(userJson);
    auto callback = [&](const HttpResponsePtr &httpResp) { resp = httpResp; };
    authController->registerUser(req, callback, move(newUser));
    // Wait for callback to execute (simplified for test)
    ASSERT_TRUE(resp);
    ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
}

TEST_F(AuthControllerTest, login_database_error) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    Json::Value userJson;
    userJson["username"] = "testuser";
    userJson["password"] = "testpass";
    HttpResponsePtr resp;
    User newUser(userJson);
    auto callback = [&](const HttpResponsePtr &httpResp) { resp = httpResp; };
    authController->loginUser(req, callback, move(newUser));
    // Wait for callback to execute (simplified for test)
    ASSERT_TRUE(resp);
    ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
}

TEST_F(AuthControllerTest, user_with_token_to_json) {
    User user;
    user.setUsername("testuser");
    user.setPassword(hashPassword("testpass"));
    UserWithToken tokenParser(user);
    Json::Value toJson = tokenParser.toJson();
    ASSERT_EQ(toJson["username"].asString(), "testuser");
    ASSERT_EQ(toJson["token"].asString(), "test-token-user_id" + to_string(user.getId()));
}

TEST_F(AuthControllerTest, are_fields_valid) {
    User validUser;
    validUser.setUsername("testuser");
    validUser.setPassword("testpass");
    EXPECT_TRUE(AuthController().areFieldsValid(validUser));
}

TEST_F(AuthControllerTest, are_fields_valid_invalid) {
    User invalidUser;
    EXPECT_FALSE(AuthController().areFieldsValid(invalidUser));
}

TEST_F(AuthControllerTest, is_user_available) {
    auto dbClient = make_shared<MockDbClient>();
    Mapper<User> mp(dbClient);
    Json::Value userJson;
    userJson["username"] = "testuser";
    User user(userJson);
    ON_CALL(*dbClient, findFutureBy(_))
        .WillByDefault(Return(std::vector<User>(1, user)));
    EXPECT_FALSE(AuthController().isUserAvailable(user, mp));
}

TEST_F(AuthControllerTest, is_user_available_empty) {
    Json::Value userJson;
    userJson["username"] = "testuser";
    User user(userJson);
    auto dbClient = make_shared<MockDbClient>();
    Mapper<User> mp(dbClient);
    ON_CALL(*dbClient, findFutureBy(_))
        .WillByDefault(Return(std::vector<User>()));
    EXPECT_TRUE(AuthController().isUserAvailable(user, mp));
}

TEST_F(AuthControllerTest, is_password_valid) {
    string plain = "testpass";
    string hash = hashPassword(plain);
    EXPECT_TRUE(AuthController().isPasswordValid(plain, hash));
    EXPECT_FALSE(AuthController().isPasswordValid("different", hash));
}