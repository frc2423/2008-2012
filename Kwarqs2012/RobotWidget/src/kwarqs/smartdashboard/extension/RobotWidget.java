/**
 * This widget shows various images that are alternatively displayed/not 
 * displayed when a particular sensor on the robot is on or off. Ideally, this 
 * should be more general... but because of time constraints it currently is 
 * not particularly generic. Or well written, for that matter :)
 */


package kwarqs.smartdashboard.extension;

import edu.wpi.first.smartdashboard.gui.DashboardFrame;
import edu.wpi.first.smartdashboard.gui.Widget;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.smartdashboard.types.DataType;
import edu.wpi.first.wpilibj.networking.NetworkListener;
import edu.wpi.first.wpilibj.networking.NetworkTable;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import javax.imageio.ImageIO;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import kwarqs.smartdashboard.types.named.RobotWidgetType;


class RobotWidgetInfo {

    public RobotWidgetInfo(String propertyName){
        String imageFilename = "./extensions/img/" + propertyName + ".png";
        this.value = false;
        try {
            this.image = ImageIO.read(new File(imageFilename));
        } catch (IOException io) {
            JOptionPane.showMessageDialog(DashboardFrame.getInstance(), "Error loading " + imageFilename + ": " + io.getMessage(), "Input Error", JOptionPane.WARNING_MESSAGE);
        }
    }
    public boolean value;           // if true, display the image     
    public BufferedImage image;     // image to display
   
};


public class RobotWidget extends Widget {
    
    public static final DataType[] TYPES = {RobotWidgetType.get()};

    private HashMap< String, RobotWidgetInfo > values;
    private BufferedImage backgroundImage;
    
    private NetworkTable table;
    private NetworkListener listener = new NetworkListener() {

        public void valueChanged(String key, Object value) {
            SwingUtilities.invokeLater(new Runnable() {
                
                // this runs on the GUI thread
                public void run() {
                    
                    // get all the values
                    for (Map.Entry<String, RobotWidgetInfo> value: values.entrySet())
                    {
                        value.getValue().value = table.getBoolean(value.getKey());
                    }
                    
                    // then force a repaint
                    repaint();
                }
            });
        }
    
        // presumably you could reject the value here... 
        public void valueConfirmed(String key, Object value) {
        }
    };
    
    // Object is a NetworkTable, this gets called probably.. once
    @Override
    public void setValue(Object value) {
        if (table != null) {
            table.removeListenerFromAll(listener);
        }
        table = (NetworkTable) value;
        table.addListenerToAll(listener);

        listener.valueChanged(null, null);
    }
    
    public void addValue( String propertyName )
    {
        values.put( propertyName, new RobotWidgetInfo(propertyName));
    }

    // initializing function
    @Override
    public void init() {

        values = new HashMap<String, RobotWidgetInfo>();
        
        try {
            backgroundImage = ImageIO.read(new File("./extensions/img/robot.png"));
            setPreferredSize(new Dimension(backgroundImage.getWidth(), backgroundImage.getHeight()));
        } catch (IOException io) {
           JOptionPane.showMessageDialog(DashboardFrame.getInstance(), "Error loading background: " + io.getMessage(), "Input Error", JOptionPane.WARNING_MESSAGE);
        }    
        
        // various settings 
        addValue("chamber");
        addValue("feeder_top");
        addValue("feeder_mid");
        addValue("feeder_low");        
        addValue("feeder_entrance");

    }

    // not sure what this does
    @Override
    public void propertyChanged(Property property) {
        
    }
    
    @Override
    protected void paintComponent(Graphics g) {
            
        if (backgroundImage == null)
            g.fillRect(0, 0, getWidth(), getHeight());
        else
            g.drawImage(backgroundImage, 0, 0, getWidth(), getHeight(), 0, 0, backgroundImage.getWidth(), backgroundImage.getHeight(), null);
        
        // draw any elements marked as shown
        for (Map.Entry<String, RobotWidgetInfo> value: values.entrySet())
        {
            RobotWidgetInfo info = value.getValue();
            
            if (info.value == true && info.image != null)
            {
                g.drawImage(info.image, 0, 0, getWidth(), getHeight(), 0, 0, backgroundImage.getWidth(), backgroundImage.getHeight(), null);
            }
        }
    }
    
}
