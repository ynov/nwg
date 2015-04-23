#include <cstdio>

#include <map>
#include <thread>
#include <mutex>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

int lastId = 0;
std::mutex mutex;
std::map<int, bool> m;

void f(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    mutex.lock();
    m[id] = true;
    mutex.unlock();

    printf("Done for [%d]!\n", id);
}

class MyHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        int id = ++lastId;
        std::thread(f, id).detach();

        session.put<int>("id", std::make_shared<int>(id));

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

        session.wait = true;
        session.waitFunction = [&mutex, &session, id](bool &wait) {
            mutex.lock();
            wait = !m[id];
            mutex.unlock();

            std::shared_ptr<Nwg::ByteBuffer> out(new Nwg::ByteBuffer());
            out->flip();

            session.write(out);

            if (!session.wait) {
                session.close();
            }
        };
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
