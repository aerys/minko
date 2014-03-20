
float
particles_normalize(float value, float lower, float upper)
{
	return clamp((value - lower)/max(0.01, upper - lower), 0.0, 1.0);
}

float
particles_velocity(vec3 	currentPosition, 
				   vec3 	oldPosition, 
				   float 	timeStep)
{
	return length(currentPosition - oldPosition) / max(1e-6, timeStep);
}

float
particles_linearlyInterpolateFloat(float 	t, 
								   float 	lower,
								   float 	lowerValue,
						 		   float 	upper,
								   float 	upperValue)
{
	return lowerValue + particles_normalize(t, lower, upper) * (upperValue - lowerValue);
}

vec3
particles_linearlyInterpolateFloat3(float 	t, 
								    float 	lower,
								    vec3 	lowerValue,
						 		    float 	upper,
								    vec3 	upperValue)
{
	return lowerValue + particles_normalize(t, lower, upper) * (upperValue - lowerValue);
}