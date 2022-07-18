#include<deque>
#include<unordered_set>
#include<unordered_map>
#include<vector>

class Project;

using namespace std;

enum class TaskStatus		// Status of the task, to be updated by Task's project.
{
	IDLE,		// Task is Idle
	RUNNING,	// Task is Running/Waiting for resource.
	FINISHED,	// Task is Finished executing
	INVALID_TASK	// The resource request by the task exceeds the max resource available at ResourceAllocater
};

enum class borrowerStatus     // Status of borrower
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
	borrowerStatus rStatus;    // allocation status from ResourceAllocater.
public:
	borrower(unsigned int borrowerId, unsigned int resourceReq);
	virtual ~borrower();
	void getId();  // return borrower Id
	void setAllocStatus(borrowerStatus status); // to be called by ResourceAllocater to change status
	virtual void execute() = 0;	// must be overrided by task.
	unsigned int getResourceReq();   // returns the required resource request.
};

class Task: public borrower
{
private:
	unsigned int userId; 	  // Id of the task user
	unsigned int timeReq;     // required time(in secs) to finish a task.
	Project& project;         // project the task belongs to.
	vector<unsigned int> depIds; // Task Ids that current task depends upon.
	TaskStatus t_stat;	  // set Task status

public:
	Task(unsigned int uId, unsigned int timeReq, unsigned int borrowerId, unsigned int resourceReq, Project& proj);
	virtual ~Task();
	
	Task& addDependency(unsigned int);
	unsigned int getTaskId();                          // return borrower Id
	vector<unsigned int>& getDependencyList(); 	   // get the dependency list from Task.
	bool checkResource();							  // check if the reource exceeds the ResourceAllocater's capacity.
	void requestResource(); 			  // requests resource from ResourceAllocater.
	void execute() override;						// to be called by ResourceAllocater once resource becomes available.
	void setTaskStatus(TaskStatus t_stat);            // to be called by the project task belongs to to change status.
	
private:
	void informResourceAllocator();				  // inform ResourceAllocator to de allocate resource once task is done executing. to be called at the end of execute.
	void informProject();				  	  // inform task's project when done executing. to be called at the end of execute. 
};

class Project
{
private:
	unsigned int pId; 			   		// project id;
	deque<Task*> taskList; 		// collective tasks which comes under a project.
public:
	Project();
	virtual ~Project();
	Project& addTask(Task*);		// add Tasks to the project fluently. eg proj1.addTask(Task1).addTask(Task2).addTask(Task3)...
	bool isExecutableInTime(time_t& targetEpoch); // checks whether given project can be executed in given time.
	bool startExecution();			// start executing projects.
	
	void doneExecuting(Task* task);         // tasks informs project once done executing.
};

//Singleton pattern, Incharge of resource pool allocates on request.
class ResourceAllocater
{
private:
	static const unsigned int totalResource;	// Total resources available.
	static ResourceAllocater* self_ptr;		// pointer to the single instance.
	deque<borrower*> requestedList;			// Requests from borrowers in order 
	unordered_set<borrower*> allocatedList;		// Requests that has been granted resources.
	
	
	ResourceAllocater(const unsigned int totRes);   // Initialise data members.
	virtual ~ResourceAllocater();			// free up all members. 
	
public:
	static ResourceAllocater* instance();		// return singleton instance of ResourceAllocater. If not present create one and send it.
	static void deinstance();			// destroy the singleton instance.
	static unsigned int getResource();		// returns totalResources available.
	
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
	Project* getProjectById(unsigned int);  // given a projectId returns Project.
	bool isProjectExecutableInTime(unsigned int projectId, time_t& targetTime); // checks whether the given project is executable in given time.
};

class LocalTimeFormat
{
private:
	// Date specifics
	unsigned int year;
	unsigned int month;
	unsigned int day;
	
	// Time Specifics
	unsigned int hour;
	unsigned int min;
	unsigned int sec;

	LocalTimeFormat() = delete;
public:
	LocalTimeFormat(unsigned int yyyy, unsigned int mm, unsigned int dd, unsigned int hr=0, unsigned int mn=0, unsigned int sc=0);
	time_t toEpoch() const;
};
