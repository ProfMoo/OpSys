import java.io.*;
import java.net.*;
import java.util.*;

public class MultiThreadServer
{
  public static void main( String[] args )
  {
    new MultiThreadServer();
  }

  public MultiThreadServer()
  {
    System.out.println( "MultiThreadServer started at " + new Date() );

    try
    {
      // Create a server socket
      ServerSocket serverSocket = new ServerSocket( 9889 );

      // Number each client connection
      int clientNumber = 1;

      while ( true )
      {
        // Listen for a new connection request
        Socket socket = serverSocket.accept();   // BLOCK

        // Display the client number
        System.out.println( "Starting thread for client " +
          clientNumber + " at " + new Date() );

        // Find the client's host name, and IP address
        InetAddress inetAddress = socket.getInetAddress();
        System.out.println( "Client " + clientNumber +
          "'s host name is " + inetAddress.getHostName() );
        System.out.println( "Client " + clientNumber +
          "'s IP Address is " + inetAddress.getHostAddress() );

        // Create a new thread for the connection
        HandleAClient thread = new HandleAClient( socket );

        // Start the new thread
        thread.start();

        // Increment clientNo
        clientNumber++;
      }
    }
    catch( IOException ex ) {
      System.err.println( ex );
    }

    System.out.println( "MultiThreadServer ended at " + new Date() );
  }


  // Inner class
  // Define the thread class for handling incoming connection
  class HandleAClient extends Thread
  {
    private Socket socket; // A connected socket

    public HandleAClient( Socket socket )
    {
      this.socket = socket;
    }

    public void run()
    {
      try {
        // Create data input and output streams
        DataInputStream inputFromClient =
          new DataInputStream( socket.getInputStream() );
        DataOutputStream outputToClient =
          new DataOutputStream( socket.getOutputStream() );

        // Continuously serve the client
        while ( true )
        {
          // Receive radius from the client
          double radius = inputFromClient.readDouble();

          // Compute area
          double area = radius * radius * Math.PI;

          // Send area back to the client
          outputToClient.writeDouble( area );

          System.out.println( "Radius received from client: " + radius );
          System.out.println( "Area found: " + area );
        }
      }
      catch( IOException ex ) {
        System.err.println( ex );
      }
    }
  }
}