import java.io.*;
import java.net.*;
import java.util.*;

public class StudentClient
{
  public static void main( String[] args )
  {
    new StudentClient( args );
  }

  public StudentClient( String[] args )
  {
    try
    {
      // Establish connection with the server
      Socket socket = new Socket( "linux02.cs.rpi.edu", 8125 );

      // Create an output stream to the server
      ObjectOutputStream toServer =
          new ObjectOutputStream( socket.getOutputStream() );

      InputStream fromServer = socket.getInputStream();

      // Create a Student object and send to the server
      Student s = new Student( args[0], args[1], Double.parseDouble( args[2] ) );
//      Student s = new Student( "John Smith", "123-45-6789", 3.5 );

      toServer.writeObject( s );
      System.out.println( "Sent student " + s.getId() + " to server" );

      byte r[] = new byte[1024];
      fromServer.read( r );
      String response = new String( r );
      System.out.println( "Rcvd from server: " + response ); // ACK or ERROR

      socket.close();
    }
    catch ( IOException ex )
    {
      System.err.println( ex );
    }
  }
}
