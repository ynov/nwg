#include <cstdio>

#include <map>
#include <thread>
#include <mutex>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

int lastId = 0;
std::mutex mutex;

std::map<int, bool> mdone;

void fn(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));

    mutex.lock();
    mdone[id] = true;
    mutex.unlock();
}

class MyHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        int id = ++lastId;

        mutex.lock();
        mdone[id] = false;
        mutex.unlock();

        std::thread(fn, id).detach();

        session.put<int>("id", std::make_shared<int>(id));
        session.put<bool>("exit", std::make_shared<bool>(false));

        std::shared_ptr<Nwg::ByteBuffer> out(new Nwg::ByteBuffer());
        out->put("Please wait... [" + std::to_string(id) + "]\n");
        out->flip();

        session.write(out);
    }

    void sessionClosed(Nwg::Session &session)
    {
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &obj)
    {
    }

    void messageSent(Nwg::Session &session, Nwg::Object &obj)
    {
        int id = session.get<int>("id");
        bool &exit = session.get<bool>("exit");

        bool done = false;

        mutex.lock();
        done = mdone[id];
        mutex.unlock();

        if (exit) {
            printf("Close!\n");
            session.close();
            return;
        }

        if (!done) {
            std::shared_ptr<Nwg::ByteBuffer> out(new Nwg::ByteBuffer());
            out->put("");
            out->flip();

            session.write(out);
            return;
        }

        std::shared_ptr<Nwg::ByteBuffer> out(new Nwg::ByteBuffer());
        out->put("OK!\n");
        out->flip();

        session.write(out);

        exit = true;
    }
};

void run()
{
    Nwg::EventLoop eventLoop;
    Nwg::Acceptor acceptor(&eventLoop, 8850);

    acceptor.setProtocolCodec(std::make_shared<Nwg::BasicProtocolCodec>());
    acceptor.setHandler(std::make_shared<MyHandler>());

    printf("Listening on port %d\n", acceptor.getPort());
    acceptor.listen();

    eventLoop.dispatch();
}

int main()
{
    run();
    return 0;
}
