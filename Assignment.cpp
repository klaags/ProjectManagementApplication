#include<iostream>
#include<ctime>

#include<deque>
#include<unordered_set>
#include<unordered_map>
#include<vector>
#include<queue>

class Project;

using namespace std;

enum class TaskStatus		// Status of the task, to be updated by Task's project.
{
	IDLE,		// Task is Idle
	RUNNING,	// Task is Running/Waiting for resource.
	FINISHED,	// Task is Finished executing
	INVALID_TASK	// The resource request by the task exceeds the max resource available at ResourceAllocater
};

enum class BorrowerStatus     // Status of borrower
{
	IDLE,		// IDLE state
	WLIST,		// Currently no resources are available.
	ALLOCATED,	// Resources are allocated to borrower.
	DEALLOCATED,	// Resources are de allocated from borrower
	INSUFF_RES	// Requested resource exceeds the total resource available.
};

class borrower
{
	
protected:
	unsigned int bId; 		   // borrower Id
	unsigned int resourceReq;  // required resource to complete task.
	BorrowerStatus rStatus;    // allocation status from ResourceAllocater.
public:
	borrower(unsigned int borrowerId, unsigned int resourceReq);
	virtual ~borrower();
	unsigned int getId();  // return borrower Id
	void setAllocStatus(BorrowerStatus status); // to be called by ResourceAllocater to change status
	virtual void execute() = 0;	// must be overrided by task.
	unsigned int getResourceReq();   // returns the required resource request.
};

class Task: public borrower
{
private:
	unsigned int userId; 	  // Id of the task user
	unsigned int timeReq;     // required time(in secs) to finish a task.
	Project& project;         // project the task belongs to.
	vector<unsigned int> dependencyList; // Task Ids that current task depends upon.
	TaskStatus t_stat;	  // set Task status

public:
	Task(unsigned int uId, unsigned int timeReq, unsigned int borrowerId, unsigned int resourceReq, Project& proj);
	virtual ~Task();
	
	Task& addDependency(unsigned int);
	unsigned int getTimeReq();					   // get time required for the task to execute.
	vector<unsigned int>& getDependencyList(); 	   // get the dependency list from Task.
	bool checkResource();							  // check if the reource exceeds the ResourceAllocater's capacity.
	void requestResource(); 			  // requests resource from ResourceAllocater.
	void execute() override;						// to be called by ResourceAllocater once resource becomes available.
	void setTaskStatus(TaskStatus t_stat);            // to be called by the project task belongs to to change status.
	
private:
	void informResourceAllocater();				  // inform ResourceAllocator to de allocate resource once task is done executing. to be called at the end of execute.
	void informProject();				  	  // inform task's project when done executing. to be called at the end of execute. 
};

class Project
{
private:
	unsigned int projectId;		   		// project id.
	unordered_map<unsigned int, Task*> taskContainer;  // maps task_id with task.
	deque<Task*> taskList; 		// collective tasks which comes under a project.
public:
	Project(unsigned int pId);
	virtual ~Project();
	Project& addTask(Task* task);		// add Tasks to the project fluently. eg proj1.addTask(Task1).addTask(Task2).addTask(Task3)...
	bool isExecutableInTime(time_t& targetEpoch); // checks whether given project can be executed in given time.
	bool startExecution();			// start executing projects.
	
	void doneExecuting(Task* task);         // tasks informs project once done executing.
};

//Singleton pattern, Incharge of resource pool allocates on request.
class ResourceAllocater
{
private:
	//static const unsigned int totalResource;	// for testing purposes. //Total resources available.
	static ResourceAllocater* self_ptr;		// pointer to the single instance.
	deque<borrower*> requestedList;			// Requests from borrowers in order 
	unordered_set<borrower*> allocatedList;		// Requests that has been granted resources.
	
	
	ResourceAllocater(); // for testing purposes //const unsigned int totRes);   // Initialise data members.
	virtual ~ResourceAllocater();			// free up all members. 
	
public:
	static unsigned int totalResource; // for testing purposes.
	static ResourceAllocater* instance();		// return singleton instance of ResourceAllocater. If not present create one and send it.
	static void deinstance();			// destroy the singleton instance.
	static unsigned int getResource();		// returns totalResources available.
	
	bool checkForSufficientResource(Task* task);  // Checks if the task's resource exceeds total resource.
	void requests(Task* task);			// Request by a task for resources.
	void releaseResource(Task* task);		// dealloc resource from a task once done executing.
};

class ProjectManager
{
private:
	static ProjectManager* self_ptr;
	unsigned int id_ctr;                               // unique id assigner
	unordered_map<unsigned int, Project*> projectContainer; // container storing project mapping to it's ID.
	
	ProjectManager();			// initialise data members;
	virtual ~ProjectManager();  // delete all projects;
public:
	static ProjectManager* instance();	// return singleton instance of ProjectManager. If not present create one and send it.
	static void deinstance();			// destroy the singleton instance
	
	unsigned int createProject();   //creates a project and assigns a unique id using id_ctr. 
	Project* getProjectById(const unsigned int);  // given a projectId returns Project.
	bool isProjectExecutableInTime(unsigned int projectId, time_t& targetTime); // checks whether the given project is executable in given time.
};

class LocalTimeFormat
{
private:
    	time_t epochTime;
	LocalTimeFormat() = delete;
public:
	// get the date/month/day from user and convert it to epoch time, hour/minute/secs are optional.
	LocalTimeFormat(unsigned int year, unsigned int month, unsigned int day, unsigned int hour=0, unsigned int min=0, unsigned int sec=0);
	time_t toEpoch() const;
};
// ---------------------------------------------------------End of declaration--------------------------------------------------------------

// -------------------------------------------------------- Start of definition-------------------------------------------------------------
/*
unsigned int ResourceAllocater::totalResource(1000);
ProjectManager* ProjectManager::self_ptr(NULL);

ProjectManager::ProjectManager(): id_ctr(0)
{}

ProjectManager::~ProjectManager()
{
    // to be implemented.
}

ProjectManager* ProjectManager::instance()
{
	if(self_ptr == NULL)
		self_ptr = new ProjectManager();
	return self_ptr;
}

void ProjectManager::deinstance()
{
	if(self_ptr != NULL)
		delete self_ptr;
}

unsigned int ProjectManager::createProject()
{
	projectContainer[id_ctr] = new Project(id_ctr);
	return id_ctr++;
}

Project* ProjectManager::getProjectById(const unsigned int id)
{
	if(projectContainer.find(id) == projectContainer.end())
		return NULL;
	return projectContainer[id];
}
*/
// project manager maintains several projects. to check if a project can be executed in a certain time we pass project's ID and the target time
// ( passed as epoch value, can be converted to epoch with help of LocalDateFormat class) as arguments. 
bool ProjectManager::isProjectExecutableInTime(unsigned int projectId, time_t& targetTime)
{
	
	if(projectContainer.find(projectId) == projectContainer.end())
		return false;
	
	return projectContainer[projectId]->isExecutableInTime(targetTime);
}
/*
Project::Project(unsigned int pId): projectId(pId)
{}

Project::~Project()
{
    // to be implemented
}

Project& Project::addTask(Task* task)
{
	taskList.push_back(task);
	taskContainer[task->getId()] = task;
	return *this;
}

bool Project::startExecution()
{
	return false; // Filler code, To be Implemented.
}

void Project::doneExecuting(Task* task)
{
	// To be Implemented.
}
*/
bool Project::isExecutableInTime(time_t& targetEpoch)
{
	/*
	Several Tasks form a project and this method checks whether all tasks execute successfully in time.
	
	The Project should be checked for cyclic dependency between tasks, If a cyclic dependency is present then the project cannot be completed. (eg: A depends on B, B depends on C, C depends on A)
	
	Topological sorting can be used to detect any cyclic dependencies, it is checked with the help of 2 Datastructures.
		- inDegreeMap: an unordered_map where key is the id of a task and value is the no of other tasks it depends upon.
		- dependencyMap: an unordered_map where key is the id of a task and value is an array of other tasks that are dependant on the key task.
	*/
	unordered_map<unsigned int, unsigned int> inDegreeMap;
	unordered_map<unsigned int, vector<unsigned int>> dependencyMap;
	for(Task* curr : taskList)
	{
		inDegreeMap[curr->getId()] = curr->getDependencyList().size();
		
		for(unsigned int id : curr->getDependencyList())
		{
			dependencyMap[id].push_back(curr->getId());
		}
	}
	
	/*
	Algorithm
		step 1: Now we use a queue to get the tasks with inDegree of 0, as they have no dependencies we will execute them first as they're independent of other tasks.  
		step 2: we push the independent tasks into a minHeap(priority_queue), if there are sufficient resources. the minHeap has the task with least execution time on top. we negate the available resources once pushed in minHeap
		step 3: we keep pushing independent tasks into minHeap till we have sufficient resources and there are independent tasks in queue.
		step 4: Now 2 cases may happen
			step 4a: queue runs out of independent tasks, in this case we execute top task from minHeap and after execution we retrieve task's resource and we reduce the dependency in inDegreeMap and check again for tasks that were dependent on it
				- if any tasks become executable(inDegree of the task becomes 0) we add it to the queue and repeat from step 2
				- else we repeat step 4a with next top task of minHeap.
			step 4b: InSufficient resources to run the task. Here we repeat step 4a except we execute process till we retrieve sufficient resource.
		step 5: when all the above steps are done for tasks and minHeap still contains tasks we execut them one by one noting the max time taken.
		step 6: finally we compare the time taken for execution with target time.
	*/
	unsigned int unfinished_tasks = taskList.size();  // we decrement these as after pushing a task into minHeap.
	unsigned int maxTime = 0;			  // to keep track of execution time.
	unsigned int totRes = ResourceAllocater::totalResource, currRes = totRes; // totalResource will be unchanged, currRes will be decremented with required resource for a task. 
	queue<pair<unsigned int, unsigned int>> independentTasks; // pair {taskId, startTime} queue for independent tasks
	
	//Initially for independent tasks the start time is 0. If a task is executed, then if other tasks dependent on it are ready for execution(inDegree = 0)
	//then the startTime of the dependent tasks are updated to the executed task's finishTime.
	
	auto logic = [](vector<unsigned int>& p1, vector<unsigned int>& p2) -> bool{  // minHeap Logic
		return p1[2] > p2[2];
	};
	priority_queue<vector<unsigned int>, vector<vector<unsigned int>>, decltype(logic)> pq(logic); // minHeap
	// vector<int> {taskId, resourceRequiredbyTask, finishingTime};
	// Task with least finishingTime will be placed on top of the heap
	
	for(pair<const unsigned int, unsigned int>& p : inDegreeMap)
	{
		if(p.second == 0)
			independentTasks.push({p.first, 0});		// adding independent taskIds, startingTime in queue.
	}
	
	while(!independentTasks.empty() || !pq.empty())
	{
		if(!independentTasks.empty())
		{
			unsigned int t_id = independentTasks.front().first, t_start_time = independentTasks.front().second;
			independentTasks.pop();
			
			Task* currTask = taskContainer[t_id];
			
			//cout << totRes << endl;
			//cout << currTask->getResourceReq() << endl; 
			if(currTask->getResourceReq() > totRes)  // check if task's resource requirement exceeds total resource if so it never can't execute.
				return false;
			
			if(currTask->getResourceReq() <= currRes) // sufficient resources for task execution.
			{
				unfinished_tasks--;
				currRes -= currTask->getResourceReq();
				pq.push({currTask->getId(), currTask->getResourceReq(), t_start_time+currTask->getTimeReq()});
			}
			else  //InSufficent resources for task execution.
			{
				unsigned int lastEndTime = 0;
				while(currRes < currTask->getResourceReq())   //execute tasks till we have enough resources
				{
					unsigned int taskId = pq.top()[0], resourceHeld = pq.top()[1];
					lastEndTime = pq.top()[2];
					pq.pop();
					
					maxTime = max(maxTime, lastEndTime);		// pop each task in minHeap and free resources.
					currRes += resourceHeld;
					
					if(dependencyMap.find(taskId) != dependencyMap.end())
					{
						for(unsigned int t : dependencyMap[taskId])
						{
							--inDegreeMap[t];
							if(inDegreeMap[t] == 0);	// tasks that can be executed now is added to the queue.
							{
								independentTasks.push({t, lastEndTime});
							}
						}
					}
				}
				
				unfinished_tasks--;
				pq.push({currTask->getId(), currTask->getResourceReq(), lastEndTime+currTask->getTimeReq()}); // add the currTask as it has enough resource.
			}
		}
		else
		{
			while(!pq.empty())   // no Independent tasks to execute.
			{
				unsigned int taskId = pq.top()[0], resourceHeld = pq.top()[1], endTime = pq.top()[2];
				pq.pop();
				
				currRes += topTask[1];
				maxTime = max(maxTime, endTime);
				
				Task* curr_task = taskContainer[taskId];
				bool taskAdded = false;
				if(dependencyMap.find(taskId) != dependencyMap.end())
				{
				    cout << endl;
					for(unsigned int t : dependencyMap[taskId])
					{
						--inDegreeMap[t];
						if(inDegreeMap[t] == 0)		// task ready to execute 
						{
							taskAdded = true;	// atleast 1 task is ready to execute now and added to queue.
							independentTasks.push({t, endTime});
						}
					}
				}
				
				if(taskAdded)
					break;
			}
		}
	}
	
	if(/*!independentTasks.empty() ||*/ unfinished_tasks != 0)  // if there are some unfinished_tasks then project can't be completed.
		return false;
	
	while(!pq.empty())
	{
		maxTime = max(maxTime, pq.top()[2]);	// check which task finishes last
	}

	time_t timeTakenEpoch = time(nullptr); // current time.
	timeTakenEpoch += maxTime;		// current time + time taken to execute the tasks.
	
	return (timeTakenEpoch <= targetEpoch);	// compare timetaken from now to target time.
}
/*
borrower::borrower(unsigned int borrowerId, unsigned int r_req):bId(borrowerId), resourceReq(r_req), rStatus(BorrowerStatus::IDLE)
{}

borrower::~borrower()
{
    // to be implemented.
}

unsigned int borrower::getId()
{
	return bId;
}

void borrower::setAllocStatus(BorrowerStatus status)
{
	// To be Implemented.
}

unsigned int borrower::getResourceReq()
{
	return resourceReq;
}

Task::Task(unsigned int uId, unsigned int t_Req, unsigned int borrowerId, unsigned int resourceReq, Project& proj): borrower(borrowerId, resourceReq), userId(uId), timeReq(t_Req),
				project(proj), t_stat(TaskStatus::IDLE)
{}

Task::~Task()
{
    // to be implemented.
}

Task& Task::addDependency(unsigned int t_id)
{
	dependencyList.push_back(t_id);
	return *this;
}

unsigned int Task::getTimeReq()
{
	return timeReq;
}

vector<unsigned int>& Task::getDependencyList()
{
	return dependencyList;
}

bool Task::checkResource()
{
	return resourceReq <= ResourceAllocater::totalResource;
}

void Task::requestResource()
{
	// to be implemented.
}

void Task::execute()
{
	//to be implemented.
}

void Task::setTaskStatus(TaskStatus t_stat)
{
	//to be implemented.
}

void Task::informResourceAllocater()
{
	//to be implemented.
}
void Task::informProject()
{
	//to be implemented.
}

LocalTimeFormat::LocalTimeFormat(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int min, unsigned int sec)
{
	struct tm t = {0};  // Initalize to all 0's
	t.tm_year = year - 1900;  // This is year-1900, eg: 2012-1900
	t.tm_mon = month - 1;     // jan = 0, feb = 1, ... dec = 11
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	
	epochTime = mktime(&t);
}

time_t LocalTimeFormat::toEpoch() const
{
    return epochTime;
}

int main()
{
    LocalTimeFormat targetTime(2022, 7, 22);
    
    //std::time_t ms = std::time(nullptr);
    
    //cout<<"Today Time: "<< targetTime.toEpoch() << endl;
    //cout<<"Curr Time: "<< ms << endl;
    //cout << (targetTime.toEpoch() <= ms) << endl;
    
    //Project*
    unsigned int pId = ProjectManager::instance()->createProject();
    Project* proj = ProjectManager::instance()->getProjectById(pId);
    
    //Task(unsigned int uId, unsigned int timeReq, unsigned int borrowerId, unsigned int resourceReq, Project& proj);
    Task* t1 = new Task(100, 500, 1, 500, *proj);
    //t1->addDependency(2).addDependency(3);
    
    Task* t2 = new Task(101, 500, 2, 500, *proj);
    //t2->addDependency(3).addDependency(1);
    
    Task* t3 = new Task(102, 800, 3, 700, *proj);
    
    proj->addTask(t1).addTask(t2).addTask(t3);
    
    time_t targEpoch = targetTime.toEpoch();
    cout << ResourceAllocater::totalResource << endl;
    cout << ProjectManager::instance()->isProjectExecutableInTime(pId, targEpoch) << endl;
    
    return 0;
}
*/
