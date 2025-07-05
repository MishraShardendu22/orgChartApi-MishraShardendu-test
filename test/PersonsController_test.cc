cpppp
#include <gtest/gtest.h>
#include <drogon/HttpServer.h>
#include <drogon/drogon.h>
#include "PersonsController.h"
#include <memory>
#include <vector>
#include <string>
#include "../models/Person.h"
#include <drogon/HttpController.h>
#include <drogon/Filter.h>
#include <drogon/trantor.h>
using namespace std;
using namespace drogon;

class PersonsControllerTest : public ::testing::Test {
protected:
    using Controller = PersonsController;

    void SetUp() override {
        // Ensure drogon app is initialized for tests
        drogon::app().getConfig()->setPort(8000);
        drogon::app().setThreadNum(4);
        drogon::app().initialize({});
        drogon::app().sched()->updateTime(0);
    }

    void TearDown() override {
        // Cleanup if necessary
        drogon::app().stop();
    }
};

TEST_F(PersonsControllerTest, GetPersons) {
    auto controller = make_shared<Controller>();
    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr &resp) {
        // Capture response for verification if needed
    };
    controller->get(req, callback);
    // Test implementation needed for async callbacks
    ASSERT_TRUE(true); // Placeholder until real test implementation
}

TEST_F(PersonsControllerTest, GetOne) {
    auto controller = make_shared<Controller>();
    auto req = HttpRequest::newHttpRequest();
    int id = 1;
    auto callback = [](const HttpResponsePtr &resp) {
        // Capture response for verification if needed
    };
    controller->getOne(req, callback, id);
    // Test implementation needed
    ASSERT_TRUE(true);
}

TEST_F(PersonsControllerTest, CreateOne) {
    auto controller = make_shared<Controller>();
    auto req = HttpRequest::newHttpRequest();
    auto person = Person{1, "John", "Doe", 30};
    auto callback = [](const HttpResponsePtr &resp) {
        // Capture response for verification if needed
    };
    controller->createOne(req, callback, move(person));
    // Test implementation needed
    ASSERT_TRUE(true);
}

TEST_F(PersonsControllerTest, UpdateOne) {
    auto controller = make_shared<Controller>();
    auto req = HttpRequest::newHttpRequest();
    int id = 1;
    auto person = Person{1, "John", "Doe", 30};
    auto callback = [](const HttpResponsePtr &resp) {
        // Capture response for verification if needed
    };
    controller->updateOne(req, callback, id, move(person));
    // Test implementation needed
    ASSERT_TRUE(true);
}

TEST_F(PersonsControllerTest, DeleteOne) {
    auto controller = make_shared<Controller>();
    auto req = HttpRequest::newHttpRequest();
    int id = 1;
    auto callback = [](const HttpResponsePtr &resp) {
        // Capture response for verification if needed
    };
    controller->deleteOne(req, callback, id);
    // Test implementation needed
    ASSERT_TRUE(true);
}

TEST_F(PersonsControllerTest, GetDirectReports) {
    auto controller = make_shared<Controller>();
    auto req = HttpRequest::newHttpRequest();
    int id = 1;
    auto callback = [](const HttpResponsePtr &resp) {
        // Capture response for verification if needed
    };
    controller->getDirectReports(req, callback, id);
    // Test implementation needed
    ASSERT_TRUE(true);
}
```