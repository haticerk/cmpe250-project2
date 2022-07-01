#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <iomanip>

using namespace std; 

static int invalidCom =0;
static int moreGift =0;

class Desk{
public:
	int id;
	float serTime;
	bool full=false;
	Desk(int _id, float _serTime){
		this->id=_id;
		this->serTime=_serTime;
	}
};

class Event{
public:
	float time=0;
	int hackerID;
	int deskID=0;
	int change=0;
	string type;

	Event(){
	}

	Event(int _hackerID, float _time){
		this->hackerID = _hackerID;
		this->time = _time;
		this->type = "stickerEnter";
	}
};

class CompareEvent{
public:
	bool operator()(const Event& event1, const Event& event2){
		if(event1.time-event2.time>0.00001){
			return true;
		}else if(abs(event1.time-event2.time)<0.00001){
			return event1.hackerID > event2.hackerID;
		}else{
			return false; 
		}
	}
};

class Commit{
public:
	int hackerID, changeLength;
	float time;
	
	Commit(int _hackerID, int _changeLength, float _time){
		this->hackerID= _hackerID;
		this->changeLength= _changeLength;
		this->time=_time;
	}
};

class Hacker{
public:	
	int id;
	float arrivingTime;
	int takenGift=0;
	float stickEnter=0; 
	float stickTaken=0;
	float hoodieEnter=0;
	float hoodieTaken=0;
	float endOfTaken=0;
	vector<Commit> commits;

	Hacker(int _id, float _arrivingTime){
		this->id = _id;
		this->arrivingTime = _arrivingTime;
	}

	~Hacker(){
	}

};

class CompareHacker{
public:
	bool operator()(const Hacker& hack1, const Hacker& hack2){
		int hack1commit=0;
		for(int i=0;i<hack1.commits.size();i++){		
			if(hack1.commits[i].changeLength > 19){
				hack1commit++;
			}
		}

		int hack2commit=0;
		for(int i=0;i<hack2.commits.size();i++){		
			if(hack2.commits[i].changeLength >= 20){
				hack2commit++;
			}
		}
		
		if(hack1commit<hack2commit){
			return true;
		}else if(hack1commit>hack2commit){
			return false;
		}else{
			if(hack1.id<hack2.id){
				return true;
			}else{
				return false;
			}
		}
	}

};


bool canEnter(Hacker *hacker, float time){
	int validCommit=0;

	if(hacker->takenGift ==3 ){
		moreGift++;
		return false;
	}

	for(int i=0; i< hacker->commits.size(); i++){	
		if(hacker->commits[i].changeLength >= 20){
			validCommit++;
		}		
	}

	if(validCommit >= 3){
		return true;
	}else{
		invalidCom++;
		return false;
	}
}

int main(int argc, char* argv[]){

	ifstream inputFile;
	inputFile.open(argv[1], ios::in);

	//Taking hacker datas.

	int hackerNum;
	inputFile >> hackerNum;

	vector<Hacker> hackers;
	for(int i=0; i<hackerNum; i++){
		float arrivingTime;
		inputFile >> arrivingTime;
		Hacker *hacker = new Hacker(i+1,arrivingTime);
		hackers.push_back(*hacker);
	}

	//To sort events by time.
	priority_queue<Event, vector<Event>, CompareEvent> allEvents;

	//Taking code commits.
	int codeCommits;
	inputFile >> codeCommits;

	for(int i=0; i<codeCommits; i++){
		int hackerID,changeLength;
		float time;
		inputFile >> hackerID >> changeLength >> time;

		Event *event = new Event(hackerID,time);
		event->type = "makeCommit";
		event->change = changeLength;
		allEvents.push(*event);		
	}
	
	//Taking queue entrance attemps.
	
	int queueEnt;
	inputFile >> queueEnt;
	
	for(int i=0; i<queueEnt; i++){
		int hackerID;
		float time;
		inputFile >> hackerID >> time;

		Event *event = new Event(hackerID, time);
		allEvents.push(*event);
	}

	//Taking both numbers and service times of desks.
	
	int numOfStickerDesk;
	inputFile>> numOfStickerDesk;
	vector<Desk> stickerDesks;

	for(int i=0; i<numOfStickerDesk; i++){
		float serviceTime;
		inputFile >> serviceTime;
		Desk *desk = new Desk(i+1, serviceTime);
		stickerDesks.push_back(*desk);
	}

	int numOfHoodieDesk;
	inputFile >> numOfHoodieDesk;
	vector<Desk> hoodieDesks;

	for(int i=0; i<numOfHoodieDesk; i++){
		float serviceTime;
		inputFile >> serviceTime;
		Desk *desk = new Desk(i+1, serviceTime);
		hoodieDesks.push_back(*desk);
	}

	inputFile.close();

	//The queue for waiting sticker.
	queue<Hacker> stickerWait;
	int maxSQueueLength=0;
	//The priority queue for waiting hoodie.
	priority_queue<Hacker, vector<Hacker>, CompareHacker> hoodieWait;
	int maxHQueueLength=0;

	//To the last event.
	Event lastEvent;

	while(!allEvents.empty()){

		Event nextEvent;
		nextEvent = allEvents.top();
		allEvents.pop();
		
		Hacker *hacker;
		for(int k=0; k<hackerNum; k++){
			if(hackers[k].id==nextEvent.hackerID){
				hacker = &hackers[k];
			}
		}
		if(nextEvent.type.compare("makeCommit")==0){
			Commit *commit = new Commit(nextEvent.hackerID, nextEvent.change, nextEvent.time);
			hacker->commits.push_back(*commit);

		}else if(nextEvent.type.compare("stickerEnter")==0){
			
			if(canEnter(hacker,nextEvent.time)){
				hacker->stickEnter += nextEvent.time;

				if(stickerWait.empty()){
					int help = allEvents.size();
	
					for(int i=0; i<stickerDesks.size(); i++){
						if(!stickerDesks[i].full){
							Event *event = new Event(hacker->id, nextEvent.time);
							event->type= "stickerTaken";
							allEvents.push(*event);
							break;
						}
					}
	
					if(allEvents.size()==help){
						stickerWait.push(*hacker);

						if(stickerWait.size() > maxSQueueLength){
							maxSQueueLength = stickerWait.size();
						}
					}

				}else{
					stickerWait.push(*hacker);

					if(stickerWait.size() > maxSQueueLength){
						maxSQueueLength = stickerWait.size();
					}
				}
			}

		}else if(nextEvent.type.compare("stickerTaken")==0){

			for(int i=0; i<numOfStickerDesk; i++){
				if(!stickerDesks[i].full){
					hacker->stickTaken += nextEvent.time;

					stickerDesks[i].full=true;

					Event *event = new Event(hacker->id, nextEvent.time + stickerDesks[i].serTime);
					event->type="hoodieEnter";
					event->deskID= stickerDesks[i].id;

					allEvents.push(*event);

					break;
				}
			}

		}else if(nextEvent.type.compare("hoodieEnter")==0){

			for(int i=0; i<numOfStickerDesk; i++){
				if(stickerDesks[i].id == nextEvent.deskID){
					stickerDesks[i].full=false;
				}
			}

			if(!stickerWait.empty()){
				Hacker *nextTake;
				nextTake = &stickerWait.front();
				stickerWait.pop();

				Event *event = new Event(nextTake->id, nextEvent.time);
				event->type = "stickerTaken";
				allEvents.push(*event);
			}

			hacker->hoodieEnter += nextEvent.time;

			if(hoodieWait.empty()){
				int help = allEvents.size();

				for(int i=0; i<numOfHoodieDesk; i++){
					if(!hoodieDesks[i].full){
						Event *event = new Event(hacker->id, nextEvent.time);
						event->type= "hoodieTaken";
						allEvents.push(*event);
						break;
					}
				}

				if(allEvents.size()==help){
					hoodieWait.push(*hacker);

					if(hoodieWait.size() > maxHQueueLength){
						maxHQueueLength = hoodieWait.size();
					}		
				}

			}else{
				hoodieWait.push(*hacker);

				if(hoodieWait.size() > maxHQueueLength){
					maxHQueueLength = hoodieWait.size();
				}
			}

		}else if(nextEvent.type.compare("hoodieTaken")==0){

			for(int i=0; i<numOfHoodieDesk; i++){
				if(!hoodieDesks[i].full){

					hacker->hoodieTaken += nextEvent.time;
					hacker->takenGift++;

					hoodieDesks[i].full=true;

					Event *event = new Event(hacker->id, nextEvent.time + hoodieDesks[i].serTime);
					event->type="exitHoodie";
					event->deskID= hoodieDesks[i].id;
					allEvents.push(*event);
					break;
				}
			}

		}else{
			hacker->endOfTaken += nextEvent.time;

			for(int i=0; i<hoodieDesks.size(); i++){
				if(hoodieDesks[i].id == nextEvent.deskID){
					hoodieDesks[i].full=false;
				}
			}

			if(!hoodieWait.empty()){
				Hacker nextTake = hoodieWait.top();
				hoodieWait.pop();

				Event *event = new Event(nextTake.id, nextEvent.time);
				event->type = "hoodieTaken";
				allEvents.push(*event);	
			}

		}

		if(allEvents.size()==1){
			lastEvent=allEvents.top();
		}

	}

 	//Writing output.
	ofstream outputFile;
	outputFile.open(argv[2]);

	//1 and 2.
	outputFile << maxSQueueLength << endl;
	outputFile << maxHQueueLength << endl;

	//3
	int totalGift=0;
	for(int i=0; i<hackers.size(); i++){
		totalGift += hackers[i].takenGift;
	}

	float avgGift = (float) totalGift / hackerNum;
	outputFile << fixed << setprecision(3) << (double) avgGift << endl;

	//4
	float totalStickerQ=0;
	for(int i=0; i<hackers.size(); i++){
		totalStickerQ = totalStickerQ + hackers[i].stickTaken - hackers[i].stickEnter;
	}

	float avgStickQueue = (float) totalStickerQ/ totalGift;
	outputFile << fixed << setprecision(3) << (double) avgStickQueue << endl; 

	//5
	float totalHoodieQ=0;
	for(int i=0; i<hackers.size(); i++){
		totalHoodieQ = totalHoodieQ + hackers[i].hoodieTaken - hackers[i].hoodieEnter;
	}

	float avgHoodieQueue = (float) totalHoodieQ/ totalGift;
	outputFile << fixed << setprecision(3) << (double) avgHoodieQueue << endl; 

	//6
	int totalCommits=0;
	for(int i=0; i<hackers.size(); i++){
		totalCommits += hackers[i].commits.size();
	}

	float avgCommits = (float) totalCommits/ hackerNum;
	outputFile << fixed << setprecision(3) << (double) avgCommits << endl;

	//7
	int totalComChange=0;
	for(int i=0; i< hackers.size(); i++){
		for(int k=0; k<hackers[i].commits.size(); k++){
			totalComChange += hackers[i].commits[k].changeLength;
		}
	}

	float avgComChange = (float) totalComChange / totalCommits;
	outputFile << fixed << setprecision(3) << (double) avgComChange << endl;

	//8
	float totalTurnAround=0;
	for(int i=0; i<hackers.size(); i++){
		totalTurnAround += hackers[i].endOfTaken - hackers[i].stickEnter;
	}

	float avgTurnAround= (float) totalTurnAround / totalGift;
	outputFile << fixed << setprecision(3) << (double) avgTurnAround << endl;

	//9
	outputFile << invalidCom << endl;
	//10
	outputFile << moreGift << endl;

	//11
	int idM=1;
	float waitMost=0;
	waitMost = hackers[0].stickTaken + hackers[0].hoodieTaken - hackers[0].stickEnter - hackers[0].hoodieEnter;
	
	for(int i=1; i< hackers.size(); i++){
		float timeGoing=0;
		timeGoing += hackers[i].stickTaken - hackers[i].stickEnter;
		timeGoing += hackers[i].hoodieTaken - hackers[i].hoodieEnter;
		
		if(timeGoing-waitMost>0.00001){
			waitMost = timeGoing;
			idM = hackers[i].id;
		}
	}

	outputFile << idM << " ";
	outputFile << fixed << setprecision(3) << (double) waitMost << endl;

	//12 
	vector<float> waitT;
	for(int i=0; i<hackers.size(); i++){
		if(hackers[i].takenGift==3){
			float whileWaiting=0;
			whileWaiting = hackers[i].stickTaken - hackers[i].stickEnter + hackers[i].hoodieTaken - hackers[i].hoodieEnter;
			
			waitT.push_back(whileWaiting);
		}else{
			waitT.push_back(-1);
		}
	}

	float waitLeast=-1;
	int idL=-1;
	for(int i=0; i< waitT.size(); i++){
		if(waitT[i]!=-1){
			idL=i+1;
			waitLeast= waitT[i];
			break;
		}
	}
	for(int i=0; i<waitT.size(); i++){
		if(waitLeast-waitT[i]>0.00001 && waitT[i]!=-1){
			waitLeast=waitT[i];
			idL=i+1;
		}
	}

	outputFile << idL << " ";
	outputFile << fixed << setprecision(3) << (double) waitLeast << endl;

	//13
 	outputFile << fixed << setprecision(3) << (double) lastEvent.time << endl;

 	outputFile.close();

 	hackers.clear();
 	stickerDesks.clear();
 	hoodieDesks.clear();
 	waitT.clear(); 

 	return 0;
}
