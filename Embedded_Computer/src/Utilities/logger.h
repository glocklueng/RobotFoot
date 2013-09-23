#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <ostream>
#include <list>
#include <time.h>

/** @addtogroup Utilities
 * @{
 */

/** \brief Class for logging data in ostream objects
 */
class Logger
{
   public:
      static Logger& getInstance();

      void addStream(std::ostream& stream);
	  
	  /** \brief Stream an object to each output stream in the stream list
	  *
	  * \param stream ostream&: Output stream to add
	  * \return Logger&: 
	  *
	  */
      template <typename T>
      Logger &operator<<(const T& object)
	  {
		  for(auto stream = _streams.begin(); stream != _streams.end(); ++stream)
		  {
			  if (_timeStamp) { **stream << timeStamp() << ": "; }
			  **stream << object;
		  }
		  _timeStamp = false;
		  return *this;
	  }

      Logger &operator<<(std::ostream& (*endlPtr)(std::ostream&));

   private:
	  bool _timeStamp;
      std::list<std::ostream*> _streams;

      Logger() { _timeStamp = true; }
      Logger(const Logger&);
      void operator=(const Logger&);

      std::string timeStamp();
};
#endif