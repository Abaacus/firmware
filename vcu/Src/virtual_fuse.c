#include "virtual_fuse.h"

#define COOLING_FACTOR              (1.0f)

#define LINEAR_CUTOFF               (1.0f)
#define NORMAL_CUTOFF               (1.0f)

#define MOTOR_POWER_SAFE            (1.0f)
#define MOTOR_LINEAR_FACTOR         (1.0f)

float cooling_factor = COOLING_FACTOR,
      linear_cutoff = LINEAR_CUTOFF,
      normal_cutoff = NORMAL_CUTOFF,
      motor_power_safe = MOTOR_POWER_SAFE,
      motor_linear_factor = MOTOR_LINEAR_FACTOR;

float get_cooling_factor()
{
    return cooling_factor;
}
void set_cooling_factor(float new_cooling_factor)
{
    cooling_factor = new_cooling_factor;
}

float get_linear_cutoff()
{
    return linear_cutoff;
}
void set_linear_cutoff(float new_linear_cutoff)
{
    linear_cutoff = new_linear_cutoff;
}

float get_normal_cutoff()
{
    return normal_cutoff;
}
void set_normal_cutoff(float new_normal_cutoff)
{
    normal_cutoff = new_normal_cutoff;
}

float get_motor_power_safe()
{
    return motor_power_safe;
}
void set_motor_power_safe(float new_motor_power_safe)
{
    motor_power_safe = new_motor_power_safe;
}

float get_motor_linear_factor()
{
    return motor_linear_factor;
}
void set_motor_linear_factor(float new_motor_linear_factor)
{
    motor_linear_factor = new_motor_linear_factor;
}

/* @brief Return the temperature factor
 * 
 * @param t_batt - battery temperature
 * @param t_amb - ambient temperature
 */
static float get_temp_factor(float t_batt, float t_amb)
{
    return ( COOLING_FACTOR * (t_batt - t_amb) );
}

/* @brief Calculate the i^2 t value and return it
 * 
 * @param i2t - original i2t value
 * @param t_batt - battery temperature
 * @param t_amb - ambient temperature
 */
static float get_modified_i2t(float i2t, float t_batt, float t_amb)
{
    return ( i2t - get_temp_factor(t_batt, t_amb) );
}

/* @brief Run the virtual fuse algorithm
 * 
 * @param i2t - original i^2t value
 * @param t_batt - battery temperature
 * @param t_amb - ambient temperature
 */
HAL_StatusTypeDef check_virtual_fuse(float i2t, float t_batt, float t_amb)
{
    float i2t_modified = get_modified_i2t(i2t, t_batt, t_amb);

    if (i2t_modified > LINEAR_CUTOFF)
    {
        // Set motor power to a safe/low value
        sendThrottleValueToMCs(MOTOR_POWER_SAFE, getSteeringAngle());
    }
    else if (i2t_modified > NORMAL_CUTOFF)
    {
        // Linearly decrease motor power
        float throttle_val;
        ThrottleStatus_t throttle_status = getNewThrottle(&throttle_val);

        if (throttle_status == THROTTLE_FAULT) {
            return HAL_ERROR;
        } else if (throttle_status == THROTTLE_DISABLED) {
            DEBUG_PRINT("Throttle disabled due to brake pressed\n");
        }

        throttle_val -= ( MOTOR_LINEAR_FACTOR * (i2t_modified - NORMAL_CUTOFF) );
        sendThrottleValueToMCs(throttle_val, getSteeringAngle());
    }
    return HAL_OK;
}
