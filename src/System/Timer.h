#ifndef __TIMER_H__
#define __TIMER_H__

#include <vector>

class TimerConfiguration {
  public:
    TimerConfiguration(uint32_t millis, uint32_t initialOffset, std::function<void()> callback): _millis(millis), _callback(callback), _timer(millis + initialOffset) {}

    void update(const uint32_t& delta) {
      _timer -= delta;
      if (_timer <= 0) {
        _callback();
        _timer += _millis;
      }
    }
private:
  uint32_t _millis;
  int32_t _timer;
  std::function<void()> _callback;
};

class Timer {
  public:
    Timer() {}

    uint32_t update() {
      uint32_t currentMillis = millis();
      auto delta = currentMillis - _lastMillis;
      _lastMillis = currentMillis;

      for (auto& deltaConfiguration: _timerConfigurations) {
        deltaConfiguration.update(delta);
      }

      return delta;
    }

    void runEvery(uint32_t millis, uint32_t initialOffset, std::function<void()> callback) {
      _timerConfigurations.push_back(TimerConfiguration(millis, initialOffset, callback));
    }
  private:
    std::vector<TimerConfiguration> _timerConfigurations;
    uint32_t _lastMillis = 0;
};

#endif
