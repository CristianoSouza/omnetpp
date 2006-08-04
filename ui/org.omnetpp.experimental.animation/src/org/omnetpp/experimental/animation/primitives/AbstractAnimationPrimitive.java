package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.figures.CompoundModuleFigure;

public abstract class AbstractAnimationPrimitive implements IAnimationPrimitive {
	protected IAnimationEnvironment animationEnvironment;
	
	protected long eventNumber;
	
	protected double beginSimulationTime;

	protected long animationNumber;
	
	protected boolean shown;
	
	public AbstractAnimationPrimitive(IAnimationEnvironment animationEnvironment, long eventNumber, double beginSimulationTime, long animationNumber) {
		this.animationEnvironment = animationEnvironment;
		this.eventNumber = eventNumber;
		this.beginSimulationTime = beginSimulationTime;
		this.animationNumber = animationNumber;
	}
	
	public long getEventNumber() {
		return eventNumber;
	}

	public double getSimulationTime() {
		return beginSimulationTime;
	}
	
	public long getAnimationNumber() {
		return animationNumber;
	}
	
	public double getBeginAnimationTime() {
		return animationEnvironment.getAnimationTimeForAnimationNumber(animationNumber);
	}

	public double getEndAnimationTime() {
		return animationEnvironment.getAnimationTimeForAnimationNumber(animationNumber + 1);
	}

	public abstract void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime);
	
	protected TimerQueue getTimerQueue() {
		return animationEnvironment.getTimerQueue();
	}

	protected IFigure getRootFigure() {
		return animationEnvironment.getRootFigure();
	}

	protected CompoundModuleFigure getCompoundModuleFigure(IRuntimeModule module) {
		return (CompoundModuleFigure)animationEnvironment.getFigure(module);
	}
	
	protected LayoutManager getLayoutManager() {
		return getRootFigure().getLayoutManager();
	}

	protected void setConstraint(IFigure figure, Rectangle constraint) {
		getLayoutManager().setConstraint(figure, constraint);
	}

	protected CompoundModuleFigure getEnclosingModuleFigure() {
		IRuntimeModule enclosingModule = animationEnvironment.getSimulation().getRootModule(); //FIXME
		CompoundModuleFigure enclosingModuleFigure = getCompoundModuleFigure(enclosingModule);
		return enclosingModuleFigure;
	}
}