package kwarqs.smartdashboard.types.named;

import edu.wpi.first.smartdashboard.types.NamedDataType;


public class RobotWidgetType extends NamedDataType {

    public static final String LABEL = "RobotWidget";

    private RobotWidgetType() {
        super(LABEL);
    }

    public static NamedDataType get() {
        if (NamedDataType.get(LABEL) != null) {
            return NamedDataType.get(LABEL);
        } else {
            return new RobotWidgetType();
        }
    }
}
