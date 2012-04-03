package edu.wpi.first.smartdashboard.types.named;

import edu.wpi.first.smartdashboard.types.NamedDataType;


public class FieldWidgetType extends NamedDataType {

    public static final String LABEL = "FieldWidget";

    private FieldWidgetType() {
        super(LABEL);
    }

    public static NamedDataType get() {
        if (NamedDataType.get(LABEL) != null) {
            return NamedDataType.get(LABEL);
        } else {
            return new FieldWidgetType();
        }
    }
}
