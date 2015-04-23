#include <cstdio>

#include <map>
#include <thread>
#include <mutex>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

int lastId = 0;
std::mutex mutex;

std::map<int, struct event *> mevent;
std::map<int, bool> mdone;

void fn(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));

    mutex.lock();
    event_add(mevent[id], NULL);
    mdone[id] = true;
    mutex.unlock();
}

class MyHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        int id = ++lastId;

        mutex.lock();
        mevent[id] = session.writeEvent;
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
        printf("Got here!\n");
        int id = session.get<int>("id");
        bool &exit = session.get<bool>("exit");

        bool done = false;

        mutex.lock();
        done = mdone[id];
        mutex.unlock();

        if (exit) {
            session.close();
            return;
        }

        if (!done) {
            event_del(session.writeEvent);
            session.x_manual = true;
            return;
        }

        session.x_manual = false;

        std::shared_ptr<Nwg::ByteBuffer> out(new Nwg::ByteBuffer());
        out->put("OK!\n");
        out->flip();

        session.write(out);

        exit = true;
    }
};

void run()
{
    Nwg::Server server(8850);

    server.setProtocolCodec(std::make_shared<Nwg::BasicProtocolCodec>());
    server.setHandler(std::make_shared<MyHandler>());

    printf("Listening on port %d\n", server.getPort());
    server.run();
}

int main()
{
    run();
    return 0;
}
