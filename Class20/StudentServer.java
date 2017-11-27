import java.io.*;
import java.net.*;
import java.util.*;

public class StudentServer
{
  private ObjectOutputStream outputToFile;

  private ObjectInputStream inputFromClient;
  private OutputStream outputToClient;

  public static void main( String[] args ) {
    new StudentServer();
  }

  public StudentServer()
  {
    try
    {
      // Create a server socket
      ServerSocket serverSocket = new ServerSocket( 8125 );
      System.out.println( "Server started at " + new Date() );

      // Create an object ouput stream to the s.dat file
      outputToFile =
        new ObjectOutputStream( new FileOutputStream( "s.dat", true ) );

      while ( true )
      {
        // Listen for a new connection request
        Socket socket = serverSocket.accept();

        // Create an input stream from the socket
        inputFromClient =
          new ObjectInputStream( socket.getInputStream() );

        outputToClient = socket.getOutputStream();
        String response;

        // Read from input
        Object object = inputFromClient.readObject();  // BLOCK

        if ( object instanceof Student )
        {
          Student s = (Student)object;
          System.out.println( "Rcvd student " + s.getId() );

          // Write to the file
          outputToFile.writeObject( object );
          System.out.println( "Wrote new student object to the file" );

          response = "ACK\n";
        }
        else
        {
          response = "ERROR\n";
        }

        byte[] b = response.getBytes();
        outputToClient.write( b );
        System.out.println( "Sent response '" + response + "' to client" );

        socket.close();
      }
    }
    catch( ClassNotFoundException ex ) {
      ex.printStackTrace();
    }
    catch( IOException ex ) {
      ex.printStackTrace();
    }
    finally {
      try {
        if ( inputFromClient != null ) inputFromClient.close();
        outputToFile.close();
      }
      catch ( Exception ex ) {
        ex.printStackTrace();
      }
    }
  }
}
