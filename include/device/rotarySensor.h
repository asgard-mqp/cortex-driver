#ifndef OKAPI_ROTARYSENSOR
#define OKAPI_ROTARYSENSOR

#include <API.h>

namespace okapi {
  class RotarySensor {
  public:
    virtual ~RotarySensor() = default;

    /**
     * Return the current sensor value
     */
    virtual int get() = 0;

    /**
     * Reset the sensor value to zero
     */
    virtual void reset() = 0;
  };
}

#endif /* end of include guard: OKAPI_ROTARYSENSOR */
