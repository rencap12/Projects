package edu.ufl.cise.cs1.controllers;
import game.controllers.AttackerController;
import game.models.*;
import java.awt.*;
import java.util.List;

public final class StudentAttackerController implements AttackerController
{
	public void init(Game game) { }

	public void shutdown(Game game) { }

	public int update(Game game,long timeDue)
	{
		int action = 1;

		int distanceToPP = Integer.MAX_VALUE;
		Node nodeClosestPP = null;
		for(Node n : game.getPowerPillList()){
			int temp = game.getAttacker().getLocation().getPathDistance(n);
			if(temp < distanceToPP ){
				nodeClosestPP = n;
				distanceToPP = temp;
			}
		}
		Node nodeClosestDefender = null;
		if(nodeClosestPP != null){ //there is power pill
			int distanceToDefender = Integer.MAX_VALUE;
			for(Defender d : game.getDefenders()) {
				if (d.getLairTime() <= 0) {
					int temp = game.getAttacker().getLocation().getPathDistance(d.getLocation());
					if (temp < distanceToDefender) {
						nodeClosestDefender = d.getLocation();
						distanceToDefender = temp;
					}
				}
			}
			if(distanceToDefender <= 10){
				//eat pill
				for (Node n : game.getPillList()) {
					Actor icon = game.getAttacker().getTargetActor(game.getDefenders(), true);
					int distance = icon.getLocation().getPathDistance(game.getAttacker().getLocation());
					if (distance < 3) {
						break;
					}
					action = game.getAttacker().getNextDir(n, true);
				}
				for(Defender d : game.getDefenders()) {
					action = game.getAttacker().getNextDir(nodeClosestPP, true);
					if (d.isVulnerable()) {
						action = game.getAttacker().getNextDir(nodeClosestDefender, true);
					}
					else {//run away from defender
						Actor icon = d.getTargetActor(game.getDefenders(), true);
						int distance = icon.getLocation().getPathDistance(game.getAttacker().getLocation());
						if (distance < 3) {
							action = game.getAttacker().getReverse();
						}
					}
				}
			}
			else{
				if(distanceToPP <= 5) {
					//remain still
					action = game.getAttacker().getReverse();
					return action;
				}
				else{//eat pill
					action = game.getAttacker().getNextDir(nodeClosestPP, true);

					for(Defender d : game.getDefenders()){
						if(d.isVulnerable() && (d.getVulnerableTime() > 2)){
							Node defenderLocation = d.getLocation();
							action = game.getAttacker().getNextDir(defenderLocation, true);
							return action;
						}
					}

				}
			}
		}
		else{//no more power pills
			int distanceToDefender = Integer.MAX_VALUE;
			for(Defender d : game.getDefenders()) {
				if (d.getLairTime() <= 0) {
					int temp = game.getAttacker().getLocation().getPathDistance(d.getLocation());
					if (temp < distanceToDefender) {
						nodeClosestDefender = d.getLocation();
						distanceToDefender = temp;
					}
				}
			}
			for (Node n : game.getPillList()) {
				action = game.getAttacker().getNextDir(n, true);
				for (Defender d : game.getDefenders()) {
					action = game.getAttacker().getNextDir(n, true);
					if (d.isVulnerable()) {
						action = game.getAttacker().getNextDir(nodeClosestDefender, true);
					}
					else{//run away from defender
						Actor icon = d.getTargetActor(game.getDefenders(),true);
						int distance = icon.getLocation().getPathDistance(game.getAttacker().getLocation());
						if(distance < 3){
							action = game.getAttacker().getReverse();
						}

					}
				}
			}
		}
		return action;
	}
}

