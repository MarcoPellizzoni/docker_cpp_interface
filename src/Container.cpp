#include "Docker.h"
#include "Shell.h"

using namespace docker;


Container::Container(CLI::Create create_command)
    :	_create_command(create_command),
    _status_names{"created", "restarting", "running", "removing", "paused", "exited", "dead" }
{
	_runtime_infos.image_name_or_id = _create_command.get_image_identifier();
	_runtime_infos.entrypoint = _create_command.get_entrypoint();
	_runtime_infos.name = _create_command.get_container_unique_name();
	_runtime_infos.current_status = "unknown";

	_notify_status_changed = []() {};
}

Container::Container(CLI::Create create_command, std::string container_unique_name)
	: _create_command(create_command),
	_status_names{ "created", "restarting", "running", "removing", "paused", "exited", "dead" }
{
	_runtime_infos.image_name_or_id = _create_command.get_image_identifier();
	_runtime_infos.entrypoint = _create_command.get_entrypoint();
	_runtime_infos.name = container_unique_name;
	_create_command.set_container_unique_name(container_unique_name);
	_runtime_infos.current_status = "unknown";
	
	_notify_status_changed = []() {};
}

Container::~Container()
{}

bool Container::operator==(const Container& other) const
{
	return (this->_runtime_infos.name == other._runtime_infos.name) || (this->_runtime_infos.ID == other._runtime_infos.ID);
}

std::ostream& Container::operator<<(std::ostream& stream)
{
	stream << "Container Infos:\n";
	stream << "{\n";
	stream << "\tName: " << _runtime_infos.name << "\n";
	stream << "\tStatus: " << _runtime_infos.current_status << "\n";
	stream << "\tImage: " << _runtime_infos.image_name_or_id << "\n";
	stream << "\tEntrypoint: " << _runtime_infos.entrypoint << "\n";
	stream << "\tContainerID: " << _runtime_infos.ID << "\n";
	stream << "}\n";

    return stream;
}

void Container::set_status_callback(std::function<void()> function)
{
    if(!function)
    {
        std::cerr << "docker::Container::set_status_callback: function is empty" << std::endl;
    }
	 _notify_status_changed = function;
    _notify_and_send_status_changed = nullptr;
	 _notify_status_changed_with_this = nullptr;
}

void Container::set_status_callback(std::function<void(Status)> function)
{
	if (!function)
	{
		std::cerr << "docker::Container::set_status_callback: function is empty" << std::endl;
	}
	_notify_status_changed = nullptr;
	_notify_and_send_status_changed = function;
	_notify_status_changed_with_this = nullptr;
}

void Container::set_status_callback(std::function<void(Container*)> function)
{
	if (!function)
	{
		std::cerr << "docker::Container::set_status_callback: function is empty" << std::endl;
	}
	_notify_status_changed = nullptr;
	_notify_and_send_status_changed = nullptr;
	_notify_status_changed_with_this = function;
}

Shell::Output Container::update_runtime_infos()
{
	Shell::Output status_ret = update_status(); // --> triggers update_status callback

	return status_ret;
}

Shell::Output Container::exec_create()
{
	Shell::Output ret = _create_command.execute();

	update_runtime_infos();

	return ret;
}

Shell::Output Container::exec_start()
{
	Shell::Output ret = CLI::Start(_runtime_infos.name).execute();

	update_runtime_infos();

	return ret;
}

Shell::Output Container::exec_stop()
{
	Shell::Output	ret = CLI::Stop(_runtime_infos.name).execute();
	
	update_runtime_infos();

	return ret;
}

Shell::Output Container::exec_remove()
{
	Shell::Output	ret = CLI::Remove(_runtime_infos.name).execute();

	if (ret.exitCode != Shell::SUCCESS)
	{
		_runtime_infos.current_status = "unknown";
		_current_status = Status::UNKNOWN;
		return ret;
	}

	_runtime_infos.ID = "";
	_runtime_infos.current_status = "removed";
	_current_status = Status::REMOVED;

    _notify_status_changed();

	return ret;
}

Shell::Output Container::exec_kill()
{
	Shell::Output	ret = CLI::Kill(_runtime_infos.name).execute();

	update_runtime_infos();

	return ret;
}

Shell::Output docker::Container::exec_destroy()
{
	Shell::Output	ret = CLI::Remove(_runtime_infos.name).force().execute();
	
	if (ret.exitCode != Shell::SUCCESS)
	{
		_runtime_infos.current_status = "unknown";
		_current_status = Status::UNKNOWN;
		return ret;
	}

	_runtime_infos.ID = "";
	_runtime_infos.current_status = "destroyed";
	_current_status = Status::REMOVED;

    _notify_status_changed();

	return ret;
}

Shell::Output Container::update_status()
{
	Status stat = Status::UNKNOWN;

	Shell::Output	ret = CLI::Inspect(_runtime_infos.name).extract(CLI::Inspect::STATUS).execute();

	if (ret.exitCode != Shell::SUCCESS)
	{
		stat = Status::UNKNOWN;
		_runtime_infos.current_status = "unknown";
		return ret;
    }

    auto it = std::find(_status_names.begin(), _status_names.end(), ret.result);

    if (it == _status_names.end())
	{
		stat = Status::UNKNOWN;
		_runtime_infos.current_status = "unknown";
		return ret;
	}

	_runtime_infos.current_status = (*it);

    long dist = std::distance(_status_names.begin(), it);

	stat = static_cast<Status>(dist);

	if (_current_status != stat)
	{
		_current_status = stat;
		if (_notify_status_changed)
		{
			_notify_status_changed(); // trigger callback
		}
		if (_notify_and_send_status_changed)
		{
			_notify_and_send_status_changed(stat); // trigger callback
		}
		if (_notify_status_changed_with_this)
		{
			_notify_status_changed_with_this(this); // trigger callback
		}
	}

	return ret;
}

Shell::Output Container::inspect_ID()
{
	Shell::Output	ret = CLI::Inspect(_runtime_infos.name).extract(CLI::Inspect::ID).execute();
	std::string id;

	if (ret.exitCode != Shell::SUCCESS)
	{
		id = "???";
		return ret;
	}

	id = ret.result;

	if (_runtime_infos.ID != id)
	{
		_runtime_infos.ID = id;
		
		// trigger callback
		//_notify_info_changed(*this, _runtime_infos);
	}

	return ret;
}


