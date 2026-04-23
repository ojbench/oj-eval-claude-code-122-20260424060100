#ifndef MEMO_HPP
#define MEMO_HPP

#include <iostream>
#include <vector>
#include <string>
#include "event.h"

inline std::string CustomNotifyLateEvent::GetNotification(int n) const {
  return NotifyLateEvent::GetNotification(n) + generator_(n);
}

class Memo {
 public:
  Memo() = delete;

  Memo(int duration) : duration_(duration), current_hour_(0) {}

  ~Memo() {}

  void AddEvent(const Event *event) {
    events_.push_back(event);
  }

  void Tick() {
    current_hour_++;
    if (current_hour_ > duration_) return;

    for (const auto* event : events_) {
      if (event->IsComplete()) continue;

      int deadline = event->GetDeadline();

      // We need to identify the type of event to know when to notify.
      if (const auto* nb = dynamic_cast<const NotifyBeforeEvent*>(event)) {
        int notify_time = nb->GetNotifyTime();
        if (current_hour_ == deadline - notify_time) {
          std::cout << event->GetNotification(0) << std::endl;
        } else if (current_hour_ == deadline) {
          std::cout << event->GetNotification(1) << std::endl;
        }
      } else if (const auto* nl = dynamic_cast<const NotifyLateEvent*>(event)) {
        int frequency = nl->GetFrequency();
        if (current_hour_ == deadline) {
          std::cout << event->GetNotification(0) << std::endl;
        } else if (current_hour_ > deadline && (current_hour_ - deadline) % frequency == 0) {
          std::cout << event->GetNotification((current_hour_ - deadline) / frequency) << std::endl;
        }
      } else {
        // NormalEvent
        if (current_hour_ == deadline) {
          std::cout << event->GetNotification(0) << std::endl;
        }
      }
    }
  }

 private:
  int duration_;
  int current_hour_;
  std::vector<const Event*> events_;
};
#endif
