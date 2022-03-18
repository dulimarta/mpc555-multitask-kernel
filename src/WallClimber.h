// $Id: WallClimber.h,v 2.5 2001/02/19 20:44:12 dulimart Exp $
#ifndef WALL_CLIMBER_H
#define WALL_CLIMBER_H

#include "MPC.h"
#include "Suction.h"
#include "Joint.h"
#include "Robot.h"
#include "stream.h"

//#include <vector>
#define FORWARD +1
#define BACKWARD -1

/**
 * WallClimber is an abstraction of the physical wall climber robot
 */
class WallClimber : public Robot {
   public:
      /// Default constructor
      WallClimber();

      /// Destructor
      ~WallClimber();

      int Climb (const int);

      /** 
       * Bring robot down while standing on Foot-1
       * @param dir direction of move (FORWARD/BACKWARD)
       */
      int FlipDownOn1 (const int dir);

      /** 
       * Bring robot down while standing on Foot-2
       * @param dir direction of move (FORWARD/BACKWARD)
       */
      int FlipDownOn2 (const int dir);

      /** 
       * Bring robot up while standing on Foot-1
       */
      int FlipUpOn1 ();

      /** 
       * Bring robot up while standing on Foot-1
       */
      int FlipUpOn2 ();

      /**
       * Turn the robot on the steering foot
       * @param angle amount of angle to turn
       */
      int Turn (float angle);

      /// Start a synchronous motion
      void BeginSync() const;

      /// Wait until a synchornous motion is completed
      void EndSync();

      /// Pull Foot-1 up
      void PullCup1 ();
      /// Pull Foot-2 up
      void PullCup2 ();
      /// Get a pointer to the MPC controller object
      void ReorderTS ();
      Controller* GetController() {
         return &cx;
      }

      /// represents the two suction cups on the robot
      Suction* cup[2];
      /// represents the three joint motors on the robot
      Joint* motor[3];
      /// represents the six touch sensors on the robot
      DSignal* touch_sensor[6]; /* touch sensors */

   protected:
      /// copy constructor is not available to public
      WallClimber(const WallClimber&);

   private:
      /// Push Foot-1 to a surface (without using touch sensors)
      int PushCup1 ();
      /// Push Foot-2 to a surface (using information from touch sensors)
      int PushCup1T ();
      /// Push Foot-2 to a surface (without using touch sensors)
      int PushCup2 ();
      /// Push Foot-2 to a surface (using information from touch sensors)
      int PushCup2T ();
      struct SyncData {
#ifdef PICBOARD
         public:
         SyncData (int i, long d, long v, long a) 
            : id(i), dist(d), vel(v), acc(a) {}
         int operator< (const SyncData& x) const {
            return abs(dist) < abs(x.dist);
         }
#endif
         int id;
         int dist, vel, acc;
      };
      float ROT_CORRECTION;

      MPC cx;
      float angleToGravity;
      float turnCorrect;
      int motion_seq;
      bool onVertical;
};
#endif
