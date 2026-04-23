#ifndef MEMO_HPP
#define MEMO_HPP

#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include "event.h"

inline std::string CustomNotifyLateEvent::GetNotification(int n) const {
  return NotifyLateEvent::GetNotification(n) + generator_(n);
}

class Memo {
  struct EventInfo {
    const Event* event;
    int type; // 0: Normal, 1: Before, 2: Late
    int param; // notify_time or frequency
  };

 public:
  Memo() = delete;

  Memo(int duration) : duration_(duration), current_hour_(0) {
    events_.reserve(1000);
  }

  ~Memo() {}

  void AddEvent(const Event *event) {
    EventInfo info;
    info.event = event;
    if (const auto* nb = dynamic_cast<const NotifyBeforeEvent*>(event)) {
      info.type = 1;
      info.param = nb->GetNotifyTime();
    } else if (const auto* nl = dynamic_cast<const NotifyLateEvent*>(event)) {
      info.type = 2;
      info.param = nl->GetFrequency();
    } else {
      info.type = 0;
      info.param = 0;
    }
    events_.push_back(info);
  }

  void Tick() {
    current_hour_++;
    if (current_hour_ > duration_) return;

    for (const auto& info : events_) {
      if (info.event->IsComplete()) continue;

      int deadline = info.event->GetDeadline();

      if (info.type == 1) { // NotifyBeforeEvent
        if (current_hour_ == deadline - info.param) {
          std::cout << info.event->GetNotification(0) << "\n";
        } else if (current_hour_ == deadline) {
          std::cout << info.event->GetNotification(1) << "\n";
        }
      } else if (info.type == 2) { // NotifyLateEvent
        if (current_hour_ == deadline) {
          std::cout << info.event->GetNotification(0) << "\n";
        } else if (current_hour_ > deadline && (current_hour_ - deadline) % info.param == 0) {
          std::cout << info.event->GetNotification((current_hour_ - deadline) / info.param) << "\n";
        }
      } else { // NormalEvent
        if (current_hour_ == deadline) {
          std::cout << info.event->GetNotification(0) << "\n";
        }
      }
    }
  }

 private:
  int duration_;
  int current_hour_;
  std::vector<EventInfo> events_;
};
#endif
