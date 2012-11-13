/*
    Copyright (c) 2011-2012 Andrey Sibiryov <me@kobology.ru>
    Copyright (c) 2011-2012 Other contributors as noted in the AUTHORS file.

    This file is part of Cocaine.

    Cocaine is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Cocaine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>. 
*/

#ifndef COCAINE_FILESYSTEM_MONITOR_DRIVER_HPP
#define COCAINE_FILESYSTEM_MONITOR_DRIVER_HPP

#include <cocaine/common.hpp>
#include <cocaine/asio.hpp>

#include <cocaine/api/driver.hpp>
#include <cocaine/api/event.hpp>

namespace cocaine { namespace driver {

struct fs_event_t:
    public engine::event_t
{
    fs_event_t(const std::string& event):
        engine::event_t(event)
    { }

    virtual
    void
    push(const void*,
         size_t)
    { }

    virtual
    void
    close() { }

    virtual
    void
    abort(error_code,
          const std::string&)
    { }
};

class fs_t:
    public api::driver_t
{
    public:
        typedef api::driver_t category_type;

    public:
        fs_t(context_t& context,
             const std::string& name,
             const Json::Value& args,
             engine::engine_t& engine);

        virtual
        ~fs_t();

        virtual
        Json::Value
        info() const;

    private:
        void
        on_event(ev::stat&, int);

    private:
        context_t& m_context;
        boost::shared_ptr<logging::logger_t> m_log;
        
        const std::string m_event,
                          m_path;

        ev::stat m_watcher;
};

}}

#endif