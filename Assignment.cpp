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
