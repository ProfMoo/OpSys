import java.io.*;
import java.net.*;
import java.util.*;

public class ClientToC
{
  // IO streams
  private OutputStream toServer;
  private InputStream fromServer;

  public static void main( String[] args )
  {
    new ClientToC();
  }

  public ClientToC()
  {
    Scanner keyboard = new Scanner( System.in );

    try
    {
      // Get the message from the user
      System.out.print( "Enter message: " );

      String s = keyboard.nextLine();
      byte[] b = s.getBytes();

      // Create a socket to connect to the server
                                   // "localhost"
      Socket socket = new Socket( "linux00.cs.rpi.edu", 8123 );

      // Create an input stream to receive data from the server
      fromServer = socket.getInputStream();

      // Create an output stream to send data to the server
      toServer = socket.getOutputStream();

      // Send the message to the server
      toServer.write( b );
      toServer.flush();
      System.out.println( "Sent message '" + s + "' to server" );

      // Get area from the server
      byte[] r = new byte[ 1024 ];
      fromServer.read( r );
      String response = new String( r );

      System.out.println( "Received from server: " + response );
    }
    catch ( IOException ex )
    {
      System.err.println( ex );
    }
  }
}
