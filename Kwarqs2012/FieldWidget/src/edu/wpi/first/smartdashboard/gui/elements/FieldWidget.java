/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.smartdashboard.gui.elements;

import edu.wpi.first.smartdashboard.gui.DashboardFrame;
import edu.wpi.first.smartdashboard.gui.Widget;
import edu.wpi.first.smartdashboard.properties.FileProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.smartdashboard.types.DataType;
import edu.wpi.first.smartdashboard.types.named.FieldWidgetType;
import edu.wpi.first.wpilibj.networking.NetworkListener;
import edu.wpi.first.wpilibj.networking.NetworkTable;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import javax.imageio.ImageIO;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;


class FieldCoordinate
{
    // robot image parameters
    private int minX;
    private int maxX;
    private int minY;
    private int maxY;
    
    // actual coordinate
    private int m_x;
    private int m_y;
    
    public boolean raised;
    public boolean selected;
    
    FieldCoordinate(int x, int y)
    {
        m_x = x;
        m_y = y;
        
        raised = false;
        selected = false;
        
        Setup(null);
    }
    
    public void Setup(BufferedImage img)
    {
        if (img != null)
        {
            minX = m_x - img.getWidth()/2;
            maxX = m_x + img.getWidth()/2;
        
            minY = m_y - img.getHeight()/2;
            maxY = m_y + img.getHeight()/2;
        }
        else
        {
            // can never hit this
            minX = 1;
            maxX = 0;
        }
        
        raised = false;
    }
    
    public boolean Hit( MouseEvent e )
    {
        return minX <= e.getX() && maxX >= e.getX() &&
               minY <= e.getY() && maxY >= e.getY();
    }
}

public class FieldWidget extends Widget implements MouseListener, MouseMotionListener {



    public static final DataType[] TYPES = {FieldWidgetType.get()};
    
    public final FileProperty fieldImageFile = new FileProperty(this, "Field Image");
    public final FileProperty robotImageFile = new FileProperty(this, "Robot Image");
    
    private BufferedImage robotImage;
    private BufferedImage fieldImage;
    
    private ArrayList<FieldCoordinate> fieldCoordinates = new ArrayList<FieldCoordinate>();
    
    private NetworkTable table;
    private NetworkListener listener = new NetworkListener() {

        public void valueChanged(String key, Object value) {
            SwingUtilities.invokeLater(new Runnable() {
                
                // this runs on the GUI thread
                public void run() {
                    
                    // parse the chosen value
                    
                    // then force a repaint
                    repaint();
                }
            });
        }

        // presumably you could reject the value here... 
        public void valueConfirmed(String key, Object value) {
        }
    };
    
    // Object is a NetworkTable
    @Override
    public void setValue(Object value) {
        if (table != null) {
            table.removeListenerFromAll(listener);
        }
        table = (NetworkTable) value;
        table.addListenerToAll(listener);

        listener.valueChanged(null, null);
    }
    
    private void initExtensions(FileProperty property)
    {
        update(property, ".");
        property.addExtensionFilter("JPEG", ".jpg");
        property.addExtensionFilter("GIF", ".gif");
        property.addExtensionFilter("PNG", ".png");
        property.addExtensionFilter("Bitmap", ".bmp");
        property.addExtensionFilter("JPEG", ".jpeg");
    }
    
    // initializing function
    @Override
    public void init() {
        
        initExtensions(fieldImageFile);
        initExtensions(robotImageFile);
        
        // setup click detection
        addMouseListener(this);
        addMouseMotionListener(this);

        // finished initializing, set a default size if no image
        if (fieldImage == null) {
            setPreferredSize(new Dimension(100, 100));
        }
        
    }
    
    private void setupCoordinates()
    {
        for( FieldCoordinate i: fieldCoordinates )
            i.Setup(robotImage);
    }
    
    @Override
    public boolean validatePropertyChange(Property property, Object value) {
        
        if (property != fieldImageFile || property != robotImageFile)
            return true;
        
        try {
            // don't care about the result
            ImageIO.read( new File((String)value));
            return true;
        } catch (IOException e) {
            JOptionPane.showMessageDialog(DashboardFrame.getInstance(), "Invalid File Type. " + e.getMessage(), "Input Error", JOptionPane.WARNING_MESSAGE);
        }
        
        return false;
    }
    
    
    @Override
    public void propertyChanged(Property property) {
        if (property == fieldImageFile) {
            try {
                fieldImage = ImageIO.read(new File(fieldImageFile.getValue()));
                setPreferredSize(new Dimension(fieldImage.getWidth(), fieldImage.getHeight()));
            } catch (IOException e) {
                fieldImage = null;
                JOptionPane.showMessageDialog(DashboardFrame.getInstance(), "Invalid Field Image File Type.", "Input Error", JOptionPane.WARNING_MESSAGE);
                setPreferredSize(new Dimension(100, 100));
            }
        }
        else if (property == robotImageFile) {
            try {
                robotImage = ImageIO.read(new File(robotImageFile.getValue()));
            } catch (IOException e) {
                robotImage = null;
                JOptionPane.showMessageDialog(DashboardFrame.getInstance(), "Invalid Robot Image File Type.", "Input Error", JOptionPane.WARNING_MESSAGE);
            }
            
            setupCoordinates();
        }
        
        repaint();
    }
    
    // mouse interaction detection
    
    @Override
    public void mouseExited(MouseEvent e) {
        
        boolean doRepaint = false;
        
        // any currently raised selections should be lowered and
        // then a repaint should be forced
        for( FieldCoordinate i: fieldCoordinates)
        {
            if (i.raised)
            {
                doRepaint = true;
                i.raised = false;
            }
        }
        
        if (doRepaint)
            repaint();
    }

    @Override
    public void mouseClicked(MouseEvent e) {
        // if a coordinate was clicked that isn't currently
        // selected, then select it and send the value over
        // to the remote listener, and then force a repaint
        boolean doRepaint = false;   
        
        for (FieldCoordinate i: fieldCoordinates)
        {
            // pick the first one
            if (!doRepaint && i.Hit(e))
            {
                i.selected = true;
                doRepaint = true;
            }
            else
            {
                i.selected = false;
            }
        }
        
        if (doRepaint)
            repaint();
    }

    @Override
    public void mouseMoved(MouseEvent e) {
        // if it's near a coordinate that isn't currently
        // selected, then note that it should be raised and
        // force a repaint
        
        boolean doRepaint = false;   
        
        for (FieldCoordinate i: fieldCoordinates)
        {
            // pick the first one
            if (!doRepaint && i.Hit(e))
            {
                i.raised = true;
                doRepaint = true;
            }
            else
            {
                i.raised = false;
            }
        }
        
        if (doRepaint)
            repaint();
    }
    
    
    // don't care about these
    
    @Override
    public void mouseEntered(MouseEvent e) {
    }

    @Override
    public void mousePressed(MouseEvent e) {
    }

    @Override
    public void mouseReleased(MouseEvent e) {
    }

    @Override
    public void mouseDragged(MouseEvent e) {
    }
    
    
    private void paintCoordinates(Graphics g)
    {
        // iterate over all of the given coordinates, draw a crosshair
        // on them. If the mouse is over one of them, draw it a little
        // bigger than the rest
    }
    
    
    @Override
    protected void paintComponent(Graphics g) {
        if (fieldImage == null)
            g.fillRect(0, 0, getWidth(), getHeight());
        else
        {
            // draw the field somewhere
            g.drawImage(fieldImage, 0, 0, getWidth(), getHeight(), 0, 0, fieldImage.getWidth(), fieldImage.getHeight(), null);
            
            // draw all current coordinates on the field so the user can see it
            paintCoordinates(g);
            
            if (robotImage != null)
            {
                // draw the robot in the selected position
            }
        }
    }
}
