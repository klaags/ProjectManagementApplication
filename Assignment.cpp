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
