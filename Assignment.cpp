enum TaskStatus		// Status of the task, to be updated by Task's project.
{
	IDLE,		// Task is Idle
	RUNNING,	// Task is Running/Waiting for resource.
	FINISHED,	// Task is Finished executing
	INVALID_TASK	// The resource request by the task exceeds the max resource available at ResourceAllocater
};

enum borrowerStatus     // Status of borrower
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
	void getId();
	virtual void setAllocStatus(borrowerStatus status) = 0;
	virtual void execute() = 0;
	unsigned int getResourceReq();   // returns the required resource request.
};

class Task: public borrower
{
private:
	unsigned int userId; 	  // Id of the task user
	unsigned int timeReq;     // required time(in secs) to finish a task.
	Project& project;         // project the task belongs to.
	vector<unsigned int> depIds // Task Ids that current task depends upon.
	TaskStatus t_stat;	  // set Task status

public:
	Task(unsigned int uId, unsigned int timeReq, Project& proj);
	Task& addDependency(unsigned int);
	unsigned int getTaskId();                          // return borrower Id
	vector<unsigned int>& getDependencyList(); 	   // get the dependency list from Task.
	bool checkResource();							  // check if the reource exceeds the ResourceAllocater's capacity.
	void requestResource(); 			  // requests resource from ResourceAllocater.
	void execute() override;						// to be called by ResourceAllocater once resource becomes available.
	void setAllocStatus(borrowerStatus status) override;                                   // to be called by ResourceAllocater to change status.
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
	bool isProjectExecutable(unsigned int projectId, time_t& targetTime); // checks whether the given project is executable in given time.
};
