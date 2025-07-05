#define DROGON_TEST_MAIN
#include <drogon/drogon_test.h>
#include <drogon/drogon.h>

DROGON_TEST(RemoteAPITest)
{
    auto client = drogon::HttpClient::newHttpClient("http://localhost:3000");
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setPath("/jobs");
    client->sendRequest(req, [TEST_CTX](drogon::ReqResult res, const drogon::HttpResponsePtr& resp) {
        REQUIRE(res == drogon::ReqResult::Ok);
        REQUIRE(resp != nullptr);
        CHECK(resp->getStatusCode() == drogon::k200OK);
        CHECK(resp->contentType() == drogon::CT_APPLICATION_JSON);
    });
}

int main(int argc, char** argv)
{
    using namespace drogon;

    std::promise<void> p1;
    std::future<void> f1 = p1.get_future();

    std::thread thr([&]() {
        app().getLoop()->queueInLoop([&p1]() { p1.set_value(); });
        app().run();
    });

    f1.get();
    int status = test::run(argc, argv);
    app().getLoop()->queueInLoop([]() { app().quit(); });
    thr.join();
    return status;
}
