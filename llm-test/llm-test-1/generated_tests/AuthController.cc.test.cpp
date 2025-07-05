```cpp
#include <gtest/gtest.h>
#include "AuthController.h"
#include "../plugins/JwtPlugin.h"

using namespace drogon::orm;
using namespace drogon_model::org_chart;

class AuthControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize mock database client and mapper
        dbClientPtr = std::make_shared<MockDbClient>();
        mapper = Mapper<User>(dbClientPtr);
        authController.setDbClient(dbClientPtr);
    }

    MockDbClient* dbClientPtr;
    Mapper<User> mapper;
    AuthController authController;
};

TEST_F(AuthControllerTest, RegisterUserWithValidData) {
    User user("testuser", "password123");
    EXPECT_CALL(*dbClientPtr, insertFuture(user)).WillOnce(Return(Future<Optional<User>>(Optional<User>(user))));
    EXPECT_CALL(mapper, findFutureBy(CompareOperator::EQ, std::string("username"), std::string("testuser"))).WillOnce(Return(Future<std::vector<User>>(std::vector<User>())));
    EXPECT_CALL(*dbClientPtr, getDbClient()).Times(1);

    auto callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
    };

    authController.registerUser(nullptr, std::move(callback), std::move(user));
}

TEST_F(AuthControllerTest, RegisterUserWithMissingFields) {
    User user("", "password123");
    EXPECT_CALL(*dbClientPtr, insertFuture(user)).Times(0);
    EXPECT_CALL(mapper, findFutureBy(CompareOperator::EQ, std::string("username"), std::string(""))).Times(0);
    EXPECT_CALL(*dbClientPtr, getDbClient()).Times(1);

    auto callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
        Json::Value ret = resp->getJsonObject();
        ASSERT_TRUE(ret.isMember("error"));
        ASSERT_EQ(ret["error"].asString(), "missing fields");
    };

    authController.registerUser(nullptr, std::move(callback), std::move(user));
}

TEST_F(AuthControllerTest, RegisterUserWithExistingUsername) {
    User user("testuser", "password123");
    EXPECT_CALL(*dbClientPtr, insertFuture(user)).Times(0);
    EXPECT_CALL(mapper, findFutureBy(CompareOperator::EQ, std::string("username"), std::string("testuser"))).WillOnce(Return(Future<std::vector<User>>(std::vector<User>(user))));
    EXPECT_CALL(*dbClientPtr, getDbClient()).Times(1);

    auto callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
        Json::Value ret = resp->getJsonObject();
        ASSERT_TRUE(ret.isMember("error"));
        ASSERT_EQ(ret["error"].asString(), "username is taken");
    };

    authController.registerUser(nullptr, std::move(callback), std::move(user));
}

TEST_F(AuthControllerTest, RegisterUserWithDatabaseError) {
    User user("testuser", "password123");
    EXPECT_CALL(*dbClientPtr, insertFuture(user)).WillOnce(Throw(std::runtime_error("Database error")));
    EXPECT_CALL(mapper, findFutureBy(CompareOperator::EQ, std::string("username"), std::string("testuser"))).Times(0);
    EXPECT_CALL(*dbClientPtr, getDbClient()).Times(1);

    auto callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
        Json::Value ret = resp->getJsonObject();
        ASSERT_TRUE(ret.isMember("error"));
        ASSERT_EQ(ret["error"].asString(), "database error");
    };

    authController.registerUser(nullptr, std::move(callback), std::move(user));
}

TEST_F(AuthControllerTest, LoginUserWithValidCredentials) {
    User user("testuser", "password123");
    EXPECT_CALL(mapper, findFutureBy(CompareOperator::EQ, std::string("username"), std::string("testuser"))).WillOnce(Return(Future<std::vector<User>>(std::vector<User>(user))));
    EXPECT_CALL(*dbClientPtr, getDbClient()).Times(1);
    EXPECT_CALL(authController.isPasswordValid("password123", "hashed_password")).WillOnce(Return(true));

    auto callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        Json::Value ret = resp->getJsonObject();
        ASSERT_TRUE(ret.isMember("username"));
        ASSERT_TRUE(ret.isMember("token"));
    };

    authController.loginUser(nullptr, std::move(callback), std::move(user));
}

TEST_F(AuthControllerTest, LoginUserWithMissingFields) {
    User user("", "password123");
    EXPECT_CALL(mapper, findFutureBy(CompareOperator::EQ, std::string("username"), std::string(""))).Times(0);
    EXPECT_CALL(*dbClientPtr, getDbClient()).Times(1);

    auto callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
        Json::Value ret = resp->getJsonObject();
        ASSERT_TRUE(ret.isMember("error"));
        ASSERT_EQ(ret["error"].asString(), "missing fields");
    };

    authController.loginUser(nullptr, std::move(callback), std::move(user));
}

TEST_F(AuthControllerTest, LoginUserWithNonExistentUser) {
    User user("nonexistentuser", "password123");
    EXPECT_CALL(mapper, findFutureBy(CompareOperator::EQ, std::string("username"), std::string("nonexistentuser"))).WillOnce(Return(Future<std::vector<User>>(std::vector<User>())));
    EXPECT_CALL(*dbClientPtr, getDbClient()).Times(1);

    auto callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
        Json::Value ret = resp->getJsonObject();
        ASSERT_TRUE(ret.isMember("error"));
        ASSERT_EQ(ret["error"].asString(), "user not found");
    };

    authController.loginUser(nullptr, std::move(callback), std::move(user));
}

TEST_F(AuthControllerTest, LoginUserWithInvalidPassword) {
    User user("testuser", "wrongpassword");
    EXPECT_CALL(mapper, findFutureBy(CompareOperator::EQ, std::string("username"), std::string("testuser"))).WillOnce(Return(Future<std::vector<User>>(std::vector<User>(user))));
    EXPECT_CALL(*dbClientPtr, getDbClient()).Times(1);
    EXPECT_CALL(authController.isPasswordValid("wrongpassword", "hashed_password")).WillOnce(Return(false));

    auto callback = [](const HttpResponsePtr &