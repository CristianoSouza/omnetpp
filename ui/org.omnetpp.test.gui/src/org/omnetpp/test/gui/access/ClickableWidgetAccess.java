package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Widget;
import org.omnetpp.test.gui.InUIThread;

public abstract class ClickableWidgetAccess<T extends Widget> extends WidgetAccess<T>
{
	public ClickableWidgetAccess(T widget) {
		super(widget);
	}

	protected abstract Point getPointToClick();

	protected abstract Menu getContextMenu();

	@InUIThread
	public void click(int button) {
		click(button, getPointToClick());
	}

	@InUIThread
	public void click() {
		click(Access.LEFT_MOUSE_BUTTON);
	}

	@InUIThread
	public void rightClick() {
		click(Access.RIGHT_MOUSE_BUTTON);
	}

	@InUIThread
	public void doubleClick() {
		doubleClick(LEFT_MOUSE_BUTTON, getPointToClick());
	}

	@InUIThread
	public MenuAccess activateContextMenuWithMouseClick() {
		rightClick();
		return new MenuAccess(getContextMenu());
	}

	/* no @InUIThread! */
	public void chooseFromContextMenu(String labelPath) {
		Assert.assertTrue("must be in a background thread for menu selection to work", Display.getCurrent()==null);

		MenuAccess menuAccess = activateContextMenuWithMouseClick();
		for (String label : labelPath.split("\\|"))
			menuAccess = menuAccess.activateMenuItemWithMouse(label);
	}

}
