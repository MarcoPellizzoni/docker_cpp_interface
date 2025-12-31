#include "docker.h"

using namespace docker;
using namespace CLI;


Shell::Output docker::CLI::destroy_all_containers()
{
	Shell s{};
	auto res = s.execute("docker ps -a --format {{.ID}}");
	if (res.exitCode != Shell::SUCCESS)
	{
		return res;
	}

	std::list<std::string> containers_IDs;
	utils::split_string(res.result, '\n', [&containers_IDs](std::string s) {containers_IDs.emplace_back(s); });
	for (auto& cnt : containers_IDs)
	{
		res = s.execute("docker rm -f " + cnt);
	}
	
	return res;
}


/***********************************
* GENERIC SHELL COMMAND
*/
I_Command::I_Command(std::string cmd)
    : _command(cmd), _p_shell(std::make_shared<Shell>())
{}

I_Command::~I_Command()
{}

std::string I_Command::str()
{
	return _command;
}

Shell::Output I_Command::execute()
{
	return _p_shell->execute(_command);
}


/***********************************
* DOCKER CREATE COMMAND
*/
Create::Create(std::string image_name)
	: I_Command("docker create"), _image_name_or_ID(image_name), _network_driver(NetworkDriver::HOST)
{}

Create::~Create()
{}

Shell::Output Create::execute()
{
	std::string exec = _command;
	exec += " --name=" + _container_name;
	exec += " --hostname=" + _container_name;
	exec += " " + _image_name_or_ID + " " + _entrypoint;
	return _p_shell->execute(exec);
}

std::string Create::str()
{
	std::string exec = _command;
	exec += " --name=" + _container_name;
	exec += " --hostname=" + _container_name;
	exec += " " + _image_name_or_ID + " " + _entrypoint;
	return exec;
}

Create& Create::remove_at_exit()
{
	_command += " -rm";
	return *this;
}

Create& Create::add_tty()
{
	_command += " -t";
	return *this;
}

Create& Create::workdir(std::string dir)
{
	_command += " -w " + dir;
	return *this;
}

Create& Create::add_dns_entry(std::string hostname, std::string hostip)
{
	_command += " --add-host=" + hostname + ":" + hostip;
	return *this;
}

Create& Create::port_map(int host_port, int container_port, NetworkProtocol protocol)
{
	std::string prot;
	switch (protocol)
	{
	case docker::CLI::Create::UDP:
		prot = "ucp";
		break;
	case docker::CLI::Create::TCP:
		prot = "tcp";
		break;
	default:
		prot = "tcp";
		break;
	}
    _command += " -p " + std::to_string(host_port);
    _command += ":" + std::to_string(container_port);
	_command += "/" + prot;
	return *this;
}

Create& Create::set_env(std::string env_name, std::string env_value)
{
	_command += " -e " + env_name + "=\"" + env_value + "\"";
	return *this;
}

Create& Create::add_external_device(std::string device_path)
{
	_command += " --device=" + device_path;
	return *this;
}

Create& Create::volume_bind_mount(std::string host_path, std::string container_path, BindMode mode)
{
	switch (mode)
	{
	case docker::CLI::Create::RO:
		_command += " --volume=\"" + host_path + ":" + container_path + ":ro" + "\"";
		break;
	case docker::CLI::Create::RW:
		_command += " --volume=\"" + host_path + ":" + container_path + ":rw" + "\"";
		break;
	default:
		_command += " --volume=\"" + host_path + ":" + container_path + ":rw" + "\"";
		break;
	}
	return *this;
}

Create& docker::CLI::Create::add_volume(std::string volume_name, std::string container_path, bool read_only)
{
	if (read_only)
	{
		_command += " --volume=\"" + volume_name + ":" + container_path + ":ro" + "\"";
	}
	else
	{
		_command += " --volume=\"" + volume_name + ":" + container_path + "\"";
	}
	return *this;
}

Create& Create::add_nvidia_gpu_support()
{
	_command += " --gpus all";
	_command += " --runtime nvidia";
	_command += " -e NVIDIA_DRIVER_CAPABILITIES=\"all\"";
	return *this;
}

Create& docker::CLI::Create::network_driver(NetworkDriver network_driver)
{
	std::string driver;
	switch (network_driver)
	{
	case docker::CLI::Create::BRIDGE:
		driver = "bridge";
		break;
	case docker::CLI::Create::HOST:
		driver = "host";
		break;
	case docker::CLI::Create::NONE:
		driver = "none";
		break;
	case docker::CLI::Create::OVERLAY:
		driver = "overlay";
		break;
	case docker::CLI::Create::IPVLAN:
		driver = "ipvlan";
		break;
	case docker::CLI::Create::MACVLAN:
		driver = "macvlan";
		break;
	default:
		break;
	}
    _command += " --network " + driver;
	return *this;
}


/***********************************
* DOCKER RUN COMMAND
*/
Run::Run(std::string image_name_or_ID)
	: Create(image_name_or_ID)
{
	_command = "docker run";
}

Run::~Run()
{
}

Run& Run::detached()
{
	_command += " -d";
	return *this;
}


/*****************************************
* DOCKER STOP COMMAND
*/
Stop::Stop(std::string container_name_or_ID)
	: I_Command("docker stop"), _container(container_name_or_ID)
{}

Stop::~Stop()
{}

Shell::Output Stop::execute()
{
	std::string exec = _command + " " + _container;
	return _p_shell->execute(exec);
}

Stop& Stop::change_contianer_to_stop(std::string container_name_or_ID)
{
	_container = container_name_or_ID;
	return *this;
}


/*****************************************
* DOCKER KILL COMMAND
*/
Kill::Kill(std::string container_name_or_ID)
	: I_Command("docker kill"), _container(container_name_or_ID)
{}

Kill::~Kill()
{}

Shell::Output Kill::execute()
{
	std::string exec = _command + " " + _container;
	return _p_shell->execute(exec);
}

Kill& Kill::change_contianer_to_kill(std::string container_name_or_ID)
{
	_container = container_name_or_ID;
	return *this;
}

/*****************************************
* DOCKER START COMMAND
*/
Start::Start(std::string container_name_or_ID)
	: I_Command("docker start"), _container(container_name_or_ID)
{}

Start::~Start()
{}

Shell::Output Start::execute()
{
	std::string exec = _command + " " + _container;
	return _p_shell->execute(exec);
}

Start& Start::change_contianer_to_start(std::string container_name_or_ID)
{
	_container = container_name_or_ID;
	return *this;
}

/*****************************************
* DOCKER REMOVE COMMAND
*/
Remove::Remove(std::string container_name_or_ID)
	: I_Command("docker rm"), _container(container_name_or_ID)
{}

Remove::~Remove()
{}

Remove& Remove::force()
{
	_command += " -f";
	return *this;
}

Remove& docker::CLI::Remove::change_contianer_to_remove(std::string container_name_or_ID)
{
	_command = container_name_or_ID;
	return *this;
}

Shell::Output Remove::execute()
{
	std::string exec = _command + " " + _container;
	return _p_shell->execute(exec);
}


/*****************************************
* DOCKER PRUNE COMMAND
*/
Prune::Prune()
	: I_Command("docker container prune -f")
{}

Prune::~Prune()
{}


/***********************************
* DOCKER IMAGES
*/
Images::Images()
	: I_Command("docker images")
{}

Images::~Images()
{}

void Images::reset_command_options()
{
	_command = "docker images";
}

Images& Images::filter(Images::Filter filter, std::string filter_value)
{
	switch (filter)
	{
	//case Images::LABEL:
	//	break;
	//case Images::BEFORE:
	//	break;
	//case Images::SINCE:
	//	break;
	case Images::REFERENCE:
		_command += " --filter \"reference=" + filter_value + "\"";
		break;
    }

	return *this;
}

Images& Images::extract(Extract ext)
{
	switch (ext)
	{
	case Images::ID:
		_command += " --format {{.ID}}";
		break;
	case Images::NAME:
		_command += " --format {{.Repository}}";
		break;
	case Images::TAG:
		_command += " --format {{.Tag}}";
		break;
    }
	return *this;
}


/***********************************
* DOCKER INSPECT CONTAINER
*/
Inspect::Inspect(std::string container_name_or_id)
	: I_Command("docker inspect " + container_name_or_id), _container(container_name_or_id)
{}

Inspect::~Inspect()
{}

void Inspect::reset_command_options()
{
	_command = "docker inspect " + _container;
}

Inspect& Inspect::extract(Extract ext)
{
	switch (ext)
	{
	case docker::CLI::Inspect::STATUS:
		_command += " --format {{.State.Status}}";
		break;
	case docker::CLI::Inspect::IMAGE_ID:
		_command += " --format {{.Config.Image}}";
		break;
	case docker::CLI::Inspect::ID:
		_command += " --format {{.Id}}";
		break;
	}

	return *this;
}

