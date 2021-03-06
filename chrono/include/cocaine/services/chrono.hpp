/*
* 2013+ Copyright (c) Alexander Ponomarev <noname@yandex-team.ru>
* All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*/

#ifndef COCAINE_CHRONO_SERVICE_HPP
#define COCAINE_CHRONO_SERVICE_HPP

#include <cocaine/api/service.hpp>
#include <cocaine/asio/reactor.hpp>
#include <cocaine/dispatch.hpp>
#include <cocaine/rpc/slots/streamed.hpp>
#include <cocaine/rpc/tags.hpp>

namespace cocaine { namespace io {

struct chrono_tag;

typedef int64_t timer_id_t;

namespace chrono {
    struct notify_after {
        typedef chrono_tag tag;

        static const char* alias() {
            return "notify_after";
        }

        typedef boost::mpl::list<
            /* time difference */ double,
            /* send id */ optional_with_default<bool, false>
        > tuple_type;

        typedef timer_id_t result_type;
    };

    struct notify_every {
        typedef chrono_tag tag;

        static const char* alias() {
            return "notify_every";
        }

        typedef boost::mpl::list<
            /* time difference */ double,
            /* send id */ optional_with_default<bool, false>
        > tuple_type;

        typedef timer_id_t result_type;
    };

    struct cancel {
        typedef chrono_tag tag;

        static const char* alias() {
            return "cancel";
        }

        typedef boost::mpl::list<
            /* timer id */ timer_id_t
        > tuple_type;

        typedef void result_type;
    };

    struct restart {
        typedef chrono_tag tag;

        static const char* alias() {
            return "restart";
        }

        typedef boost::mpl::list<
            /* timer id */ timer_id_t
        > tuple_type;

        typedef void result_type;
    };
}

template<>
struct protocol<chrono_tag> {
    typedef mpl::list<
        chrono::notify_after,
        chrono::notify_every,
        chrono::cancel,
        chrono::restart
    > type;

    typedef boost::mpl::int_<
        1
    >::type version;
};

namespace aux {
    template<class R>
    struct select<streamed<R>> {
        template<class Event>
        struct apply {
            typedef io::streamed_slot<streamed<R>, Event> type;
        };
    };
}

} // namespace io

namespace service {

class chrono_t:
    public api::service_t,
    public implementation<io::chrono_tag>
{
    public:
        chrono_t(context_t& context,
                 io::reactor_t& reactor,
                 const std::string& name,
                 const Json::Value& args);

        virtual
        dispatch_t&
        prototype() {
            return *this;
        }

    private:
        struct timer_desc_t {
            std::shared_ptr<ev::timer> timer_;
            std::shared_ptr<streamed<io::timer_id_t>> promise_;
        };

        streamed<io::timer_id_t>
        notify_after(double time, bool send_id);

        streamed<io::timer_id_t>
        notify_every(double time, bool send_id);

        void
        cancel(io::timer_id_t timer_id);

        void
        restart(io::timer_id_t timer_id);

        void 
        on_timer(ev::timer &w, int revents);

        void
        remove_timer(io::timer_id_t timer_id);
        
    private:
        streamed<io::timer_id_t>
        set_timer_impl(double first, double repeat, bool send_id);

        std::shared_ptr<logging::log_t> log_;
        std::map<io::timer_id_t, timer_desc_t > timers_;
        std::map<ev::timer*, io::timer_id_t> timer_to_id_;
        ev::timer update_timer_;
        cocaine::io::reactor_t& reactor_;
};

}} // namespace cocaine::service

#endif
