/**
    @file      Docker.h
    @brief     Interface API to execute docker commands and manage Docker Containers as a C++ Object
    @details   ~ 
    @author    Marco Pellizzoni
**/
#pragma once

#if defined(WIN32) && defined(_BUILD_DOCKER_LIB_DLL)
	#if defined(_DOCKER_LIB_EXPORT)
		#define DOCKERAPI __declspec(dllexport)
	#else
		#define DOCKERAPI __declspec(dllimport)
	#endif
#else
	#define DOCKERAPI
#endif

#include "Shell.h"
#include <string>
#include <array>
#include <set>
#include <memory>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <functional>


namespace docker
{
	namespace CLI
	{
		/**
			@brief  Kills and remove all container that are present on the current system
			@retval  - Command execution exit status and standard output result
		**/
		DOCKERAPI Shell::Output destroy_all_containers();

		/**

			@class   I_Command
			@brief   A class representing a Shell generic command.
			@details ~ Construct the class with the command to execute and call the method to execute the command in the system Shell.

		**/
		class DOCKERAPI I_Command
		{
		protected:
			std::string _command;
			std::shared_ptr<Shell> _p_shell;
		public:
			I_Command(std::string cmd);
			virtual ~I_Command();

			/**
				@brief  Get the currently constructed command as a string
				@retval  - 
			**/
			virtual std::string str();

			/**
				@brief  Executes the command through the shell interface
				@retval  - Command execution exit status and standard output result
			**/
			virtual Shell::Output execute();

			/**
				@brief Erases all the added command options to reset the command to the basic one
			**/
			virtual void reset_command_options() {};
		};

		/**
		
			@class   Create
			@brief   Docker Create command. After a succesfull execution, the container will be in "created" state.
			@details ~ Creates a Docker container without starting it.

		**/
		class DOCKERAPI Create : public I_Command
		{
		public:
			/**
				@brief Creates the command starting from the specified image name
				@param image_name_or_ID - a docker image present in the current system
			**/
			Create(std::string image_name_or_ID);
			~Create();
			
			/**
				@brief Change the docker image from which creating the container
				@param image_name_or_ID - 
			**/
			void change_image(std::string& image_name_or_ID) { _image_name_or_ID = image_name_or_ID; }

			/**
				@brief  Get the image unique alfanumeric identifier assigned by the docker engine
				@retval  - The image ID
			**/
			std::string get_image_identifier() { return _image_name_or_ID; }

			/**
				@brief Set the name of the container. This must be a unique name, i.e., no other existing container can have the same name
				@param container_name - The name of the container
			**/
			void set_container_unique_name(std::string& container_name) { _container_name = container_name; }

			/**
				@brief  Get the current container name
				@retval  - The container name
			**/
			std::string get_container_unique_name() { return _container_name; }

			/**
				@brief Set the command that will be executed when the container will start. It can be a shell script, a shell command, an executable and anything that can be called from a shell and that lives inside the container.
				@param entrypoint - The equivalent command the you would write on the command line
			**/
			void set_entrypoint(std::string& entrypoint) { _entrypoint = entrypoint; }

			/**
				@brief  Get the current entrypoint
				@retval  - The entrypoint
			**/
			std::string get_entrypoint() { return _entrypoint; }

			/**
				@brief  Executes the docker command with all the added options 
				@retval  - Exit code and standard output resulting from the command execution
			**/
			Shell::Output execute() override;

			/**
				@brief  Get the current composed command
				@retval  - 
			**/
			std::string str() override;

			/**
				@brief  Option to make docker engine delete the container when this will be stopped or killed.
				@retval  - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& remove_at_exit();

			/**
				@brief  Option to attach a pseudo TTY to the container, meaning that you get access to input-output features that TTY devices provide.
				@retval  - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& add_tty();

			/**
				@brief  The directory inside the container from which the container will execute the entrypoint.
				@param  dir - Full path to the working directory
				@retval     - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& workdir(std::string dir);
			
			/**
				@brief  Add a DNS entry to the local DNS of the container, meaning that the give Name will be resolved with the corresponding given IP
				@param  hostname - Name of the remote host
				@param  hostip   - IP of the remote host
				@retval	         - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& add_dns_entry(std::string hostname, std::string hostip);

			enum NetworkProtocol
			{
				TCP,
				UDP,
			};
			/**
				@brief  Map the given host port with an internal port. 
						WARNING: This option may be used only when the selected network driver option is of type BRIDGE.
				@param  host_port      - External port, i.e., the port exposed to the world using the network device of the host machine 
				@param  container_port - The internal port of the container that any running process may use to 
				@retval	               - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& port_map(int host_port, int container_port, NetworkProtocol protocol = TCP);

			/**
				@brief  Add an environmet variable to the container filesystem
				@param  env_name  - 
				@param  env_value - 
				@retval           - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& set_env(std::string env_name, std::string env_value);

			/**
				@brief  Attach a device to the container filesystem (anything that is under the /dev/ folder on linux filesystem)
				@param  device_path - Full name of the device
				@retval             - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& add_external_device(std::string device_path);

			enum BindMode
			{
				RW,
				RO
			};
			/**
				@brief  Mount a volume (like a folder or a drive) that belongs to the host filesystem. The container gains access to the host data present
						at this host path.
				@param  host_path      - Full path of the directory of the host machine
				@param  container_path - Full path where to mount the volume in the container filesystem
				@param  read_only      - The folder is mounted as read-only, meaning that only the host machine has permission to write data.
				@retval                - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& volume_bind_mount(std::string host_path, std::string container_path = 0, BindMode mode = RW);

			/**
				@brief  Mount a volume (like a folder or a drive) for the container to be shared with others containers and/or to used to 
						store data and make it persistent when the container is stopped and possibly restarted. The host machine cannot access this volume.
				@param  volume_name    - Name of the folder to mount
				@param  container_path - Absolute path in the container filesystem where to mount. Note that if the container path already exists in the filesystem of the image, it will be overwrite
				@param  read_only      - The folder is mounted as read-only. Usefull if the container needs only to read data that is written by another container
				@retval                - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& add_volume(std::string volume_name, std::string container_path = 0, bool read_only = false);

			/**
				@brief  Enables all nvidia gpu capabilities. This option is bound to the installation and configuration of the nvidia container toolkit
				@retval  - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& add_nvidia_gpu_support();
			
			enum NetworkDriver
			{
				BRIDGE,
				HOST,
				NONE,
				OVERLAY,
				IPVLAN,
				MACVLAN
			};
			/**
				@brief  Set which type of network driver use for the container.
						BRIDGE: The container attaches to a virtual network created by docker engine with and internal ip. 
								Docker engine acts as a NAT forwarding output packets
								Incoming packets are routed to the container only if ports where exposed (port_map option)
						HOST:	The container shares the same IP and network driver of the host. Packets routing acts as if the
								container and the host where the same entity.
						NONE:	The container remains isolated from the network
						OVERLAY:Used to connect multiple docker daemons (engines) together
						IPVLAN:	The container can have its own ip address (see , gateway and subnetmask acting like an indipendent entity in the network
						MACVLAN:Equals the the IPVLAN but it also can have it own mac address
				@param  network_driver - Driver to use
				@retval                - The instance of the command object itself. This way you can call the following command option in a pipeline fashon.
			**/
			Create& network_driver(NetworkDriver network_driver); // TODO: add options like the ip , mac address and other stuff (see docker documentation)

		private:
			std::string _image_name_or_ID;
			std::string _entrypoint;
			std::string _container_name;
			NetworkDriver _network_driver;
		};

		/**
		 
			@class   Run
			@brief   Docker run command. Creates a Docker container and immediatly starts it.
			@details ~ After a succesfull execution, the container will be in "running" state.
					 By defualt the container is started in attach mode, meanig that it attaches to the current shell.

		**/
		class DOCKERAPI Run : public Create
		{
		public:
			/**
				@brief Creates the command starting from the specified image name
				@param image_name_or_ID - a docker image present in the current system
			**/
			Run(std::string image_name_or_ID);
			~Run();

			/**
				@brief  Option to run the container in detached mode (as a background process)
				@retval  - The instance of the command object itself. This way you can call the following method in a pipeline fashon.
			**/
			Run& detached();
		};

		/**

			@class   Start
			@brief   Docker Start command. Starts a previously created container with the Create class.
			@details ~ After a succesfull execution, the container will be in "running" state.
					 By defualt the container is started in detach mode, meanig that it runs in the background.

		**/
		class DOCKERAPI Start : public I_Command
		{
			std::string _container;
		public:

			/**
				@brief Construct the command giving the container name/ID to start
				@param container_name_or_ID - 
			**/
			Start(std::string container_name_or_ID);
			~Start();

			/**
				@brief  Executes the docker command.
				@retval  - Exit code and standard output resulting from the command execution.
			**/
			Shell::Output execute() override;

			/**
				@brief  Set a new container name or ID to start.
				@param  container_name_or_ID - The assigned unique name or ID of the docker container.
				@retval                      - The instance of the command object itself. This way you can call the following method in a pipeline fashon.
			**/
			Start& change_contianer_to_start(std::string container_name_or_ID);
		};

		/**

			@class   Stop
			@brief   Docker Stop command. Stops a running container. 
			@details ~ After a succesfull execution, the container will be in "Exited" state.

		**/
		class DOCKERAPI Stop : public I_Command
		{
			std::string _container;
		public:
			/**
				@brief Construct the command giving the container name/ID to stop.
				@param container_name_or_ID - The assigned unique name or ID of the docker container.
			**/
			Stop(std::string container_name_or_ID);
			~Stop();

			/**
				@brief  Executes the docker command.
				@retval  - Exit code and standard output resulting from the command execution.
			**/
			Shell::Output execute() override;

			/**
				@brief  Set a new container name or ID to stop.
				@param  container_name_or_ID - The assigned unique name or ID of the docker container.
				@retval                      - The instance of the command object itself. This way you can call the following method in a pipeline fashon.
			**/
			Stop& change_contianer_to_stop(std::string container_name_or_ID);
		};

		/**

			@class   Kill
			@brief   Docker Kill command. Brutally stops a running container.
			@details ~ After a succesfull execution, the container will be in "Exited" state.

		**/
		class DOCKERAPI Kill : public I_Command
		{
			std::string _container;
		public:

			/**
				@brief Construct the command giving the container name/ID to kill.
				@param container_name_or_ID - The assigned unique name or ID of the docker container.
			**/
			Kill(std::string container_name_or_ID);
			~Kill();

			/**
				@brief  Executes the docker command.
				@retval  - Exit code and standard output resulting from the command execution.
			**/
			Shell::Output execute() override;

			/**
				@brief  Set a new container name or ID to kill.
				@param  container_name_or_ID - The assigned unique name or ID of the docker container.
				@retval                      - The instance of the command object itself. This way you can call the following method in a pipeline fashon.
			**/
			Kill& change_contianer_to_kill(std::string container_name_or_ID);
		};

		/**

			@class   Remove
			@brief   Docker Remove command. Deletes the container from the system.
			@details ~ After a succesfull execution, the container will no longer exists on the system.
			         The Contaner Object of this API will be in the non official docker state "removed".

		**/
		class DOCKERAPI Remove : public I_Command
		{
			std::string _container;
		public:

			/**
				@brief Construct the command giving the container name/ID to remove.
				@param container_name_or_ID - The assigned unique name or ID of the docker container.
			**/
			Remove(std::string container_name_or_ID);
			~Remove();

			/**
				@brief  Executes the docker command.
				@retval  - Exit code and standard output resulting from the command execution.
			**/
			Shell::Output execute() override;

			/**
				@brief  Add this option to brutally destroy the container.
				@retval  - The instance of the command object itself. This way you can call the following method in a pipeline fashon.
			**/
			Remove& force();

			/**
				@brief  Set a new container name or ID to remove.
				@param  container_name_or_ID - The assigned unique name or ID of the docker container.
				@retval                      - The instance of the command object itself. This way you can call the following method in a pipeline fashon.
			**/
			Remove& change_contianer_to_remove(std::string container_name_or_ID);
		};

		/**

			@class   Images
			@brief   Docker Images command. Lists all images present on the system.
			@details ~ The output consists on a table where the rows are the images and the colums are the image details.
					 Colums are: 
					 1. REPOSITORY - is the name of image
					 2. TAG - is a unique tag (typcally a version)
					 3. IMAGE ID - the unique alphanumeric ID
					 4. CREATED - elapsed time since creation
					 5. SIZE - Occupied size in the non-volatile memory storage.

		**/
		class DOCKERAPI Images : public I_Command
		{
		public:
			Images();
			~Images();

			/**
				@brief Reset command to default.
			**/
			void reset_command_options() override;

			enum Filter
			{
				//LABEL, BEFORE, SINCE, 
				REFERENCE
			};
			/**
				@brief  Apply a filer to the output table. 
						TODO: for now the only filter available is REFERENCE.
				@param  filter       - Selected filter from the enum to apply
				@param  filter_value - Value of the filter
				@retval              - The instance of the command object itself. This way you can pipeline a multiple filters and extractions and then execute.
			**/
			Images& filter(Filter filter, std::string filter_value);
			
			enum Extract
			{
				ID, NAME, TAG,
			};
			/**
				@brief  Clean the output result extracting one of the informations selected from the enum
						ID - gets the ID of the image
						NAME - gets the repository name
						TAG - gest the tag
				@param  ext - What to extract
				@retval     - The instance of the command object itself. This way you can pipeline a multiple filters and extractions and then execute.
			**/
			Images& extract(Extract ext);
		};

		/**

			@class   Inspect
			@brief   Docker Inspect command. Check the status of the container
			@details ~ Provides all information of an existing container.

		**/
		class DOCKERAPI Inspect : public I_Command
		{
			std::string _container;
		public:

			/**
				@brief Construct the command giving the container name/ID to inspect.
				@param container_name_or_ID - The assigned unique name or ID of the docker container.
			**/
			Inspect(std::string container_name_or_id);
			~Inspect();

			/**
				@brief Resets to default. Cleans all options. 
			**/
			void reset_command_options() override;

			enum Extract
			{
				STATUS,	IMAGE_ID, ID,
			};
			/**
				@brief  Extract an informations of insterest selected from the enum
				@param  ext - What to extract
				@retval     - The instance of the command object itself. This way you can pipeline the call to the execute.
			**/
			Inspect& extract(Extract ext);
		};

		/**

			@class   Prune
			@brief   Removes all containers that are in the "exited" state.
			@details ~ Containers that are not in the exited state will not be affected.

		**/
		class DOCKERAPI Prune : public I_Command
		{
		public:
			Prune();
			~Prune();
		};
	}


	/**

		@class   Container
		@brief   A class representing the instance of a docker container. 
		@details ~ Create the container object using a create command, act on the container using the exec_ methods, get runtime informations
		         and get notification upon status changes providing a callback function.

	**/
	class DOCKERAPI Container
	{
	public:

		/**
			@struct RuntimeInfos
			@brief  This struct represents the relevant informations of a docker container
		**/
		struct RuntimeInfos
		{
            std::string image_name_or_id;
            std::string name;
            std::string ID;
            std::string entrypoint;
            std::string	current_status;
		};

		/**
			@enum  docker::Container::Status
			@brief REMOVED and UNKNOWN are not official docker states. Are used only for internal purpose.
		**/
		enum class Status
		{
			UNKNOWN = -2,
			REMOVED = -1,
			CREATED,
			RESTARTING,
			RUNNING,
			REMOVING,
			PAUSED,
			EXITED,
			DEAD,
		};

		/**
			@brief Creates the object representing a container
			@param create_command - The create command to use to create the container.
		**/
		explicit Container(CLI::Create create_command);

		/**
			@brief Creates the object representing a container
			@param create_command        - The create command to use to create the container.
			@param container_unique_name - The unique name or id of the container. [WARNING] the uniqueness is mandatory for the actual creation of the docker container.
		**/
		explicit Container(CLI::Create create_command, std::string container_unique_name);
		~Container();
		Container(const Container&) = delete;
		Container& operator=(const Container&) = delete;
		//Container(Container&&); // TODO
		//Container&& operator=(Container&&); // TODO
        bool operator==(const Container& other) const;
		std::ostream& operator<<(std::ostream& stream);

        /**
            @brief Set the callback function. The function will be called every time the status of the container changes, i.e., when the status of
			       the docker container is different from the status of the container object.
            @param fun - Function to call
        **/
        void set_status_callback(std::function<void()> fun);

		/**
			@brief  The current status of the constainer object. It may be different from the
			        actual status of the docker container. Always update its value by calling update_status.
			@retval  - 
		**/
		Status		get_status()			{ return _current_status; }

		CLI::Create	get_create_command()	{ return _create_command; }

		RuntimeInfos get_runtime_infos()	{ return _runtime_infos; }

		/**
			@brief  Create the container executing the Create command passed the the constructor.
					Status will change from UNKNOWN to CREATED (or UNKNOWN if unsuccesfull execution).
			@retval  - Exit code and standard output resulting from the command execution.
		**/
		Shell::Output exec_create();

		/**
			@brief  Start the container. [WARNING] Need to call exec_create first!
					Status will change from CREATED to RUNNING (or UNKNOWN if unsuccesfull execution).
			@retval  - Exit code and standard output resulting from the command execution.
		**/
		Shell::Output exec_start();


        /**
            @brief  Run the container, attached to video. [WARNING] Need to call exec_create first!
                    Status will change from CREATED to RUNNING (or UNKNOWN if unsuccesfull execution).
            @retval  - Exit code and standard output resulting from the command execution.
        **/
        Shell::Output exec_run();

		/**
			@brief  Stops the container. [WARNING] Need to call exec_start first!
					Status will change from RUNNING to EXITED (or UNKNOWN if unsuccesfull execution).
			@retval  - Exit code and standard output resulting from the command execution.
		**/
		Shell::Output exec_stop();

		/**
			@brief  Kills the container. [WARNING] Need to call exec_start first!
					Status will change from EXITED to REMOVED (or UNKNOWN if unsuccesfull execution).
			@retval  - Exit code and standard output resulting from the command execution.
		**/
		Shell::Output exec_kill();

		/**
			@brief  Removes the container. [WARNING] Need to call exec_stop or exec_kill first!
					Status will change from EXITED to REMOVED (or UNKNOWN if unsuccesfull execution).
			@retval  - Exit code and standard output resulting from the command execution.
		**/
		Shell::Output exec_remove();
		
		/**
			@brief  Brute force destroys the container.
					Status will become REMOVED (or UNKNOWN if unsuccesfull execution).
			@retval  - Exit code and standard output resulting from the command execution.
		**/
		Shell::Output exec_destroy();

		/**
			@brief  Check the status of the docker container and updates the status of the container object.
			        If status has changed, triggers the provided function callback.
					Status will become REMOVED (or UNKNOWN if unsuccesfull execution).
			@retval  - Exit code and standard output resulting from the command execution.
		**/
		Shell::Output update_status();

		/**
			@brief  Retrives the ID of the docker container and updates the ID in the runtime informations.
					If unsuccesfull execution, the ID will be "???".
			@retval  - Exit code and standard output resulting from the command execution.
		**/
		Shell::Output inspect_ID();

	private:
		Shell::Output update_runtime_infos();

		RuntimeInfos _runtime_infos;
		CLI::Create	_create_command;
		Status _current_status = Status::UNKNOWN;
		std::array<std::string, 7> _status_names;
		std::function<void()> _notify_status_changed;
	};

    // UTILIY FUNCTIONS
    namespace utils
    {
        template<typename EmplaceFunc>
        void split_string(std::string& str, const char delimeter, EmplaceFunc f)
        {
            std::istringstream istream(str);

            std::string substring;
            while (std::getline(istream, substring, delimeter))
            {
                f(substring);
            }

            return;
        }

    }

}
