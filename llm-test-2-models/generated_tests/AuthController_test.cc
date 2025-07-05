```cpp
#include <gtest/gtest.h>
#include <drogon/HttpController.h>
#include <drogon/drogon.h>
#include <drogon/orm/Mapper.h>
#include "../models/User.h"
#include "../plugins/JwtPlugin.h"
#include <string>
#include <vector>
#include <json/json.h>
#include <optional>

using namespace testing;

class AuthControllerTest : public Test {
protected:
    void SetUp() override {
        auto app = drogon::app();
        app->setLogLevel(drogon::LogLevel::Trace);
    }
};

TEST_F(AuthControllerTest, RegisterUserMissingFields) {
    auto req = HttpRequest::newHttpRequest();
    auto user = User::newEmpty();
    AuthController controller;
    auto callback = [](const HttpResponsePtr &resp) {
        auto responseJson = resp->getJsonBody();
        EXPECT_EQ(responseJson["error"].asString(), "missing fields");
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
    };
    controller.registerUser(req, callback, std::move(user));
}

TEST_F(AuthControllerTest, RegisterUserUsernameTaken) {
    // Setup mock database to return non-empty user
    auto req = HttpRequest::newHttpRequest();
    auto user = User::newEmpty();
    user.setValueOfUsername("testuser");
    AuthController controller;
    
    auto callback = [](const HttpResponsePtr &resp) {
        auto responseJson = resp->getJsonBody();
        EXPECT_EQ(responseJson["error"].asString(), "username is taken");
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
    };

    // This test requires underlying db mocking that's not shown
    // We'd need to mock the db layer with gmock in a full test suite
    controller.registerUser(req, callback, std::move(user));
}

TEST_F(AuthControllerTest, RegisterUserSuccessful) {
    auto req = HttpRequest::newHttpRequest();
    auto user = User::newEmpty();
    user.setValueOfUsername("testuser");
    user.setValueOfPassword("securepassword123");

    auto callback = [](const HttpResponsePtr &resp) {
        auto responseJson = resp->getJsonBody();
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
        ASSERT_TRUE(responseJson["token"].isString());
        ASSERT_TRUE(responseJson["username"].isString());
        auto token = AuthController::UserWithToken(user)->toJson();
        // More comprehensive token validation would go here
        EXPECT_TRUE(token["token"].isString());
        EXPECT_EQ(token["username"].asString(), "testuser");
    };

    AuthController controller;
    controller.registerUser(req, callback, std::move(user));
}

TEST_F(AuthControllerTest, LoginUserMissingFields) {
    auto req = HttpRequest::newHttpRequest();
    auto user = User::newEmpty();
    AuthController controller;
    auto callback = [](const HttpResponsePtr &resp) {
        auto responseJson = resp->getJsonBody();
        EXPECT_EQ(responseJson["error"].asString(), "missing fields");
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
    };
    controller.loginUser(req, callback, std::move(user));
}

TEST_F(AuthControllerTest, LoginUserUserNotFound) {
    // Requires db mocking
    // Setup to return empty user array
    auto req = HttpRequest::newHttpRequest();
    auto user = User::newEmpty();
    AuthController controller;
    
    auto callback = [](const HttpResponsePtr &resp) {
        auto responseJson = resp->getJsonBody();
        EXPECT_EQ(responseJson["error"].asString(), "user not found");
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
    };

    // This test requires underlying db mocking
    controller.loginUser(req, callback, std::move(user));
}

TEST_F(AuthControllerTest, LoginUserPasswordMismatch) {
    // Requires db mocking
    // Setup to return a user, but validate against it
    auto req = HttpRequest::newHttpRequest();
    auto user = User::newEmpty();
    user.setValueOfUsername("testuser");
    user.setValueOfPassword(bcrypt::hash("wrongpassword"));

    AuthController controller;
    auto callback = [](const HttpResponsePtr &resp) {
        auto responseJson = resp->getJsonBody();
        EXPECT_EQ(responseJson["error"].asString(), "username and password do not match");
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k401Unauthorized);
    };
    
    // This test requires underlying db mocking
    controller.loginUser(req, callback, std::move(user));
}

TEST_F(AuthControllerTest, UserWithTokenToJson) {
    auto user = User::newEmpty();
    user.setValueOfUsername("testuser");
    auto tokenData = AuthController::UserWithToken(user);
    auto json = tokenData.toJson();
    
    EXPECT_TRUE(json.isObject());
    EXPECT_TRUE(json["username"].isString());
    EXPECT_EQ(json["username"].asString(), "testuser");
    EXPECT_TRUE(json["token"].isString());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```