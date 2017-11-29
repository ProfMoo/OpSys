import java.io.*;
import java.net.*;
import java.util.*;

public class Client
{
  // IO streams
  private DataOutputStream toServer;
  private DataInputStream fromServer;

  public static void main( String[] args )
  {
    new Client();
  }

  public Client()
  {
    Scanner keyboard = new Scanner( System.in );

    try
    {
      while ( true )
      {
        // Attempt to create a socket and connect to the server
        // Socket socket = new Socket( "localhost", 9889 );
        Socket socket = new Socket( "OPSYS", 9889 );
        // Socket socket = new Socket( "128.113.126.29", 9889 );

        // Create an input stream to receive data from the server
        fromServer = new DataInputStream( socket.getInputStream() );

        // Create an output stream to send data to the server
        toServer = new DataOutputStream( socket.getOutputStream() );

        // Get the radius from the user (probably want to move this
        // above the Socket object creation, i.e., the connect-to-server)
        System.out.print( "Enter radius: " );
        double radius = keyboard.nextDouble();

        // Send the radius to the server
        toServer.writeDouble( radius );
        toServer.flush();
        System.out.println( "Sent radius " + radius + " to server" );

        // Get area from the server
        double area = fromServer.readDouble();   // BLOCK

        // Display to the text area
        System.out.println( "Area received from server is " + area );
      }
    }
    catch ( IOException ex )
    {
      System.err.println( ex );
    }
  }
}
