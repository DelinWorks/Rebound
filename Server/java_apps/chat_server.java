import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;
import java.util.ArrayList;

class SharedDatatype<T> {
    private T shared;
    public T get() {
        return this.shared;
    }
    public void set(T newValue) {
        this.shared = newValue;
    }
}

class NetThread implements Runnable {
    Socket s;
    int i;
    SharedDatatype<ArrayList<NetThread>> clients;
    SharedDatatype<String> messages;
    public PrintWriter pw = null;

    NetThread(Socket s, int i, SharedDatatype<ArrayList<NetThread>> clients, SharedDatatype<String> messages) {
        this.s = s;
        this.i = i;
        this.clients = clients;
        this.messages = messages;
    }

    public void run() {
        try
        {
            pw = new PrintWriter(s.getOutputStream(), true);
            Scanner sc = new Scanner(s.getInputStream());
            String m = messages.get();

            if (m.length() > 0)
                pw.println(m);

            byte[] buffer = new byte[1024];

            while (true) {
                try {

                    if (sc.hasNext()) {
                        String msg = sc.nextLine();
                        messages.set(messages.get() + msg + "\n");
                        for (var c : clients.get())
                        {
                            if (c.pw != null)
                                c.pw.println(msg);
                        }
                    }
                } catch (Exception e) {
                    clients.get().remove(this);
                    System.out.println("Client " + s.getRemoteSocketAddress() + " Left.");
                }
            }
        } catch (Exception ex) {}
    }
}

class EchoServerMultipleClients {
    public static void main(String[] args) throws Exception {
        ServerSocket ss = new ServerSocket(1337);
        System.out.println("Chat Server is Running");
        int i = 0;
        SharedDatatype<ArrayList<NetThread>> clients = new SharedDatatype<ArrayList<NetThread>>();
        SharedDatatype<String> messages = new SharedDatatype<String>();
        clients.set(new ArrayList<NetThread>());
        messages.set("");

        while (true) {
            Socket s = ss.accept();
            System.out.println("Connection Established from: " + s.getRemoteSocketAddress().toString());
            NetThread r = new NetThread(s, i, clients, messages);
            clients.get().add(r);
            Thread t = new Thread(r);
            t.start();
            i++;
        }
    }
}
