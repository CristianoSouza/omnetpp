package com.simulcraft.test.gui.access;

import static java.lang.Math.PI;
import static java.lang.Math.abs;
import static java.lang.Math.max;
import static java.lang.Math.sin;
import static java.lang.Math.sqrt;

import com.simulcraft.test.gui.core.AnimationEffects;
import com.simulcraft.test.gui.core.UIStep;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;

public class ClickableAccess
	extends Access 
{
    public static int delayBeforeMouseMove = 500;
    public static int delayAfterMouseMove = 500;
    public static int mouseMoveDurationMillis = 500;
    public static int mouseMoveMaxSteps = 30;

    // mouse trajectory: two functions fx(t),fy(t) needed, f(0)=0 and f(1)=1 for both, 0<=t<1
    interface Ft01 { double f(double t); };
    
    //private Ft01 xt = new Ft01() { public double f(double t) { return pow(t, 0.2); } };
    //private Ft01 yt = new Ft01() { public double f(double t) { return pow(t, 0.2); } };

    //private Ft01 xt = new Ft01() { public double f(double t) { return sin(t*PI/2) + sin(t*2*PI)/20; } };
    //private Ft01 yt = new Ft01() { public double f(double t) { return sin(t*PI/2) - sin(t*2*PI)/20; } };

    //private Ft01 xt = new Ft01() { public double f(double t) { return sin(t*PI/2) + sin(t*PI)/10; } };
    //private Ft01 yt = new Ft01() { public double f(double t) { return sin(t*PI/2); } };

    private Ft01 xt = new Ft01() { public double f(double t) { return sin(t*PI/2); } };
    private Ft01 yt = new Ft01() { public double f(double t) { return sqrt(sin(t*PI/2)); } };

	@UIStep
	public void moveMouseAbsolute(Point p) {
		moveMouseAbsolute(p.x, p.y);
	}
	
	@UIStep
	public void moveMouseAbsolute(int x, int y) {
	    Point p = Display.getCurrent().getCursorLocation();
	    if (p.x != x || p.y != y) {
            postMouseEvent(SWT.MouseMove, 0, p.x, p.y, Access.rescaleTime(delayBeforeMouseMove));
	        if (mouseMoveDurationMillis > 0) {
	            log(debug, "moving mouse smoothly from "+p+" to "+new Point(x,y));
                int steps = max(abs(x-p.x), abs(y-p.y));
	            if (steps > mouseMoveMaxSteps) steps = mouseMoveMaxSteps;
	            int stepMillis = max( (int)(Access.rescaleTime(mouseMoveDurationMillis) / steps), 1);
	            double dt = 1.0 / steps;
	            for (double t = 0; t < 1.0; t += dt)
	                postMouseEvent(SWT.MouseMove, 0, p.x + (int)(xt.f(t)*(x-p.x)), p.y + (int)(yt.f(t)*(y-p.y)), stepMillis);
	        }
	        postMouseEvent(SWT.MouseMove, 0, x, y, Access.rescaleTime(delayAfterMouseMove));
	    }
	}

	@UIStep
	public void clickAbsolute(int button, Point point) {
		clickAbsolute(button, point.x, point.y);
	}

	@UIStep
	public void clickAbsolute(int button, int x, int y) {
	    moveMouseAbsolute(x, y);
		postMouseEvent(SWT.MouseDown, button, x, y);
		postMouseEvent(SWT.MouseUp, button, x, y);
	}

    @UIStep
    public void clickCenterAbsolute(int button, Rectangle rectangle) {
        clickAbsolute(button, getCenter(rectangle));
    }

	@UIStep
	public void doubleClickAbsolute(int button, int x, int y) {
	    moveMouseAbsolute(x, y);
        postMouseEvent(SWT.MouseDown, button, x, y);
        postMouseEvent(SWT.MouseUp, button, x, y);
		postMouseEvent(SWT.MouseDown, button, x, y);
		postMouseEvent(SWT.MouseUp, button, x, y);
	}

	@UIStep
	public void doubleClickAbsolute(int button, Point point) {
		doubleClickAbsolute(button, point.x, point.y);
	}

	@UIStep
	public void doubleClickCenterAbsolute(int button, Rectangle rectangle) {
		doubleClickAbsolute(button, getCenter(rectangle));
	}
	   
	@UIStep
	public void dragMouseAbsolute(int button, Point fromPoint, Point toPoint) {
	    dragMouseAbsolute(button, fromPoint.x, fromPoint.y, toPoint.x, toPoint.y);
	}

	@UIStep
	public void dragMouseAbsolute(int button, int x, int y, int x2, int y2) {
        moveMouseAbsolute(x, y);

        postMouseEvent(SWT.MouseDown, button, x, y);

        AnimationEffects.beginAnimateDragDrop(x, y, x2, y2);
        moveMouseAbsolute(x2, y2);
        AnimationEffects.endAnimateDragDrop(x, y, x2, y2);

        postMouseEvent(SWT.MouseUp, button, x2, y2);
	}

    public static int getMouseMoveDurationMillis() {
        return mouseMoveDurationMillis;
    }

    public static void setMouseMoveDurationMillis(int mouseMoveDurationMillis) {
        ClickableAccess.mouseMoveDurationMillis = mouseMoveDurationMillis;
    }

}
