#ifndef MEMO_HPP
#define MEMO_HPP

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <functional>
#include "event.h"

inline std::string CustomNotifyLateEvent::GetNotification(int n) const {
  return NotifyLateEvent::GetNotification(n) + generator_(n);
}

class Memo {
  struct ScheduledItem {
    int hour;
    int event_idx;
    int n;

    bool operator>(const ScheduledItem& other) const {
      if (hour != other.hour) return hour > other.hour;
      return event_idx > other.event_idx;
    }
  };

  struct EventInfo {
    const Event* event;
    int type; // 0: Normal, 1: Before, 2: Late
    int param; // notify_time or frequency
  };

 public:
  Memo() = delete;

  Memo(int duration) : duration_(duration), current_hour_(0) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
  }

  ~Memo() {}

  void AddEvent(const Event *event) {
    EventInfo info;
    info.event = event;
    int deadline = event->GetDeadline();
    int idx = static_cast<int>(events_.size());

    if (const auto* nb = dynamic_cast<const NotifyBeforeEvent*>(event)) {
      info.type = 1;
      info.param = nb->GetNotifyTime();
      events_.push_back(info);

      if (info.param > current_hour_ && info.param <= duration_) {
        pq_.push({info.param, idx, 0});
      }
      if (deadline > current_hour_ && deadline <= duration_) {
        pq_.push({deadline, idx, 1});
      }
    } else if (const auto* nl = dynamic_cast<const NotifyLateEvent*>(event)) {
      info.type = 2;
      info.param = nl->GetFrequency();
      events_.push_back(info);

      if (deadline > current_hour_) {
        if (deadline <= duration_) {
          pq_.push({deadline, idx, 0});
        }
      } else {
        int n = (current_hour_ - deadline + info.param - 1) / info.param;
        int t = deadline + n * info.param;
        if (t <= duration_) {
          pq_.push({t, idx, n});
        }
      }
    } else {
      info.type = 0;
      info.param = 0;
      events_.push_back(info);

      if (deadline > current_hour_ && deadline <= duration_) {
        pq_.push({deadline, idx, 0});
      }
    }
  }

  void Tick() {
    current_hour_++;
    if (current_hour_ > duration_) return;

    while (!pq_.empty() && pq_.top().hour == current_hour_) {
      ScheduledItem item = pq_.top();
      pq_.pop();

      if (!events_[item.event_idx].event->IsComplete()) {
        std::cout << events_[item.event_idx].event->GetNotification(item.n) << "\n";

        if (events_[item.event_idx].type == 2) { // Late
          int next_hour = current_hour_ + events_[item.event_idx].param;
          if (next_hour <= duration_) {
            pq_.push({next_hour, item.event_idx, item.n + 1});
          }
        }
      }
    }
  }

 private:
  int duration_;
  int current_hour_;
  std::vector<EventInfo> events_;
  std::priority_queue<ScheduledItem, std::vector<ScheduledItem>, std::greater<ScheduledItem>> pq_;
};
#endif
