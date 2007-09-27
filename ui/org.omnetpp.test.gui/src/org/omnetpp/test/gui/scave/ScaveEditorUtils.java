package org.omnetpp.test.gui.scave;

import org.omnetpp.test.gui.access.ScaveEditorAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;

public class ScaveEditorUtils {

	public static ScaveEditorAccess openAnalysisFile(String project, String analysisFile) {
        WorkbenchUtils.findInProjectExplorerView(project+"/"+analysisFile).reveal()
        	.activateContextMenuWithMouseClick()
        		.activateMenuItemWithMouse(".*Open With.*")
        			.activateMenuItemWithMouse(".*Analysis.*");
		return findScaveEditor(analysisFile);
	}
	
	public static ScaveEditorAccess findScaveEditor(String analysisFile) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		MultiPageEditorPartAccess editor = workbenchWindow.findMultiPageEditorPartByTitle(analysisFile); 
		return editor instanceof ScaveEditorAccess ? (ScaveEditorAccess)editor : null;
	}
}
