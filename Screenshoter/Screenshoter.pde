import java.awt.Robot; //<>// //<>//
import java.awt.AWTException;
import java.awt.event.InputEvent;
import java.awt.image.DataBufferInt;
import java.awt.image.BufferedImage;
import java.awt.Rectangle;
import java.awt.Dimension;
import processing.serial.*;

// Creates object "port" of serial class
Serial port; 
// Creates object "robby" of robot class
Robot robby; 

// Screen width in pixels
final int WIDTH = 1368; 
// Screen height in pixels
final int HEIGHT = 928; 

// We only need to create these objects once
Dimension dim = new Dimension(WIDTH, HEIGHT);
Rectangle rec = new Rectangle(dim);
BufferedImage screenshot;

// Some calculations we only need once
int area = WIDTH * HEIGHT;

void setup()
{
  String[] list = Serial.list();
  // Exit as this is a fatal error
  if (list.length <= 0) {
    println("Failed to receive a serial port"); 
    exit();
  }
  // Show us the ports that have been found
  for(int i = 0; i < list.length; i++) {
     println("Port " + i + " " + list[i]); 
  }
  // And then find the active serial port 
  String portName; 
  for (int i = 0; i < list.length; i++) {
    try {
      portName = list[i];
      // If we find a com port, connect immediatly
      if(portName.toLowerCase().contains("tty.usbmodem") || (portName.toLowerCase().contains("com") & !portName.toLowerCase().contains("/dev"))) {
        port = new Serial(this, portName, 9600); // Set baud rate
        if (port != null) {
          println("Connected to: " + portName);
          break;
        }
      }
    }
    catch (Throwable err) {
      println("Error opening port: " + err);
      exit();
    }
  }

  // Window size (doesn't matter)
  size(200, 200); 
  try 
  {
    robby = new Robot();
  }
  catch (AWTException e)
  {
    println("Robot class not supported by your system!");
    exit();
  }
}

void draw()
{
  // Capture the screen image (Buffered Image object)
  screenshot = robby.createScreenCapture(rec);

  // Now loop throught he pixel data all at once
  int r = 0, g = 0, b = 0;
  final int[] pixels = ((DataBufferInt) screenshot.getRaster().getDataBuffer()).getData();
  for (int i = 0; i < pixels.length; i++) {
    r += (int)(255&(pixels[i]>>16)); // Add up reds
    g += (int)(255&(pixels[i]>>8)); // Add up greens
    b += (int)(255&(pixels[i])); // Add up blues
  }

  // Average the values
  r /= area; // Average red
  g /= area; // Average green
  b /= area; // Average blue

  // Write the data to the port
  if(port != null) {
    port.write(0xff); // Write marker (0xff) for synchronization
    port.write((byte)(r)); // Write red value
    port.write((byte)(g)); // Write green value
    port.write((byte)(b)); // Write blue value
  }
  
  // Make window background average color
  background(r, g, b); 
}