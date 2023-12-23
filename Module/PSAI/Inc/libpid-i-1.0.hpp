/*********************************************************************************
  *FileName:		PID.hpp
  *Author:  		qianwan
  *Detail: 			PID template

  *Version:  		2.0
  *Date:  			2023/12/24
  *Describe:		Add forward loop

  *Version:  		1.3
  *Date:  			2023/05/17
  *Describe:		Add forward loop
  
  *Version:  		1.2
  *Date:  			2023/03/13
  *Describe:		修补BUG
  
  *Version:  		1.1
  *Date:  			2023/02/26
  *Describe:		调整积分微分范围判定条件表达式

  *Version:  		1.0
  *Date:  			2023/02/10
  *Describe:		基于测试数据,取消对CMSIS-DSP的依赖
**********************************************************************************/
#pragma once
#ifndef PID_H_
#define PID_H_
namespace PID {

    template<typename T>
    class cPID {
    protected:
        virtual T ForwardLoop() = 0;

    public:
        virtual void SetRef(T ref) = 0;

        virtual T Calculate(T fdb) = 0;

        virtual void Rst() = 0;
    };

    template<typename T>
    class cPID_Inc : public cPID<T> {
    protected:
        /*PID Parameters*/
        T _kp; /*Ratio*/
        T _ki; /*Integral*/
        T _kd; /*Differentiation*/
        T _kf; /*Forward*/
        T _dt; /*Timing difference*/
        T _out; /*Frequency*/
        T _max_out;
        T _min_out;
        bool _en_inter_separation;  /*Enable integral separation*/
        bool _en_differ_separation; /*Enable differentiation separation*/
        T _inter_range;  /*Integral Separation Range*/
        T _differ_range; /*Differentiation Separation Range*/

        /*PID Template Value*/
        T _feedback;
        T _ref;
        T _last_ref;
        T _error;
        T _derror;
        T _dderror;
        T _preerror;
        T _prederror;

        T ForwardLoop() override {
            /* 1阶惯性环节前馈环 G(s)=(1+s)/Ts */
            return (_ref - _last_ref + 1) / _dt;
        }

    public:
        cPID_Inc(
                T kp,
                T ki,
                T kd,
                T kf,
                T dt,
                T max_out,
                T min_out,
                bool en_inter_separation,
                T inter_range,
                bool en_differ_separation,
                T differ_range
        ) :
                _kp(kp),
                _ki(ki),
                _kd(kd),
                _kf(kf),
                _dt(dt),
                _max_out(max_out),
                _min_out(min_out),
                _en_inter_separation(en_inter_separation),
                _en_differ_separation(en_differ_separation),
                _inter_range(inter_range),
                _differ_range(differ_range) {
            Rst();

        }

        void Rst() override {
            _feedback = 0;
            _ref = 0;
            _last_ref = 0;
            _error = 0;
            _derror = 0;
            _dderror = 0;
            _prederror = 0;
            _preerror = 0;
            _out = 0;
        }

        void SetRef(T ref) override {
            _last_ref = _ref;
            _ref = ref;
        }

        T Calculate(T fdb) override {
            /*中间量*/
            T tmp[4];

            /*前期准备*/
            _feedback = fdb;
            _error = _ref - _feedback;
            _derror = _error - _preerror;
            _dderror = _derror - _prederror;
            _preerror = _error;
            _prederror = _derror;

            /*比例积分微分运算*/
            //pid->Out = pid->Out + (pid->Kp * pid->DError + pid->Ki * pid->Error + pid->Kd * pid->DDError);
            tmp[0] = _kp * _derror;
            //I 积分分离
            if (_en_inter_separation ? (fabs(_error) < _inter_range) : 0) {
                tmp[1] = _ki * _error;
            }
            //D 微分分离
            if (_en_differ_separation ? (fabs(_error) < _differ_range) : 0) {
                tmp[2] = _kd * _dderror;
            }

            //求和
            tmp[3] = _out + tmp[0] + tmp[1] + tmp[2] + ForwardLoop() * _kf;

            /*后期处理*/
            //输出限幅
            tmp[3] = (tmp[3] > _max_out) ? _max_out : tmp[3];
            tmp[3] = (tmp[3] < _min_out) ? _min_out : tmp[3];
            //赋值
            _out = tmp[3];
            return tmp[3];
        }
    };
}
//
//float cPIDInc::PID_Cal(float fdb) {
//    /*中间量*/
//    float tmp[0] = 0.0f;
//    float tmp[1] = 0.0f;
//    float tmp[2] = 0.0f;
//    float tmp[3] = 0.0f;
//
//    /*前期准备*/
//    _feedback = fdb;
//    _error = _ref - _feedback;
//    _derror = _error - _preerror;
//    _dderror = _derror - _prederror;
//    _preerror = _error;
//    _prederror = _derror;
//
//    /*比例积分微分运算*/
//    //pid->Out = pid->Out + (pid->Kp * pid->DError + pid->Ki * pid->Error + pid->Kd * pid->DDError);
//    tmp[0] = _kp * _derror;
//    //I 积分分离
//    if ((IN_RANGE_EN_I == 0.0f) ? 1 : (fabs(_error) < this->IN_RANGE_EN_I)) { tmp[1] = _ki * _error; }
//    //D 微分分离
//    if ((IN_RANGE_EN_D == 0.0f) ? 1 : (fabs(_error) < this->IN_RANGE_EN_D)) { tmp[2] = _kd * _dderror; }
//
//    //求和
//    tmp[3] = _out + tmp[0] + tmp[1] + tmp[2] + this->ForwardLoop() * this->Kf;
//
//    /*后期处理*/
//    //输出限幅
//    tmp[3] = (tmp[3] > _max_out) ? _max_out : tmp[3];
//    tmp[3] = (tmp[3] < _min_out) ? _min_out : tmp[3];
//    //赋值
//    _out = tmp[3];
//    return tmp[3];
//}
//
///* 1阶惯性环节前馈环 G(s)=(1+s)/Ts */
//float cPIDInc::ForwardLoop(void) {
//    return (_ref - this->LastRef + 1) * this->Fs;
//}
//
//void cPIDInc::Reset(void) {
//    _feedback = 0;
//    _error = 0;
//    _derror = 0;
//    _dderror = 0;
//    _preerror = 0;
//    _prederror = 0;
//    _out = 0;
//    this->LastRef = 0;
//}
//
//
///*位置式PID*/
//float cPIDPla::PID_Cal(float fdb) {
//    /*中间量*/
//    float tmp[0] = 0.0f;
//    float tmp[1] = 0.0f;
//    float tmp[2] = 0.0f;
//    float tmp[3] = 0.0f;
//
//    /*前期准备*/
//    _feedback = fdb;
//    _error = _ref - _feedback;
//    this->integral += _error;
//
//    //积分限幅
//    this->integral = (this->integral > this->Maxintegral) ? this->Maxintegral : this->integral;
//    this->integral = (this->integral < this->Minintegral) ? this->Minintegral : this->integral;
//
//    /*比例积分微分运算*/
//    //pid->Out = pid->Kp * pid->Error + pid->Ki * pid->integral  + pid->Kd * (pid->Error - pid->DError);
//    //P
//    tmp[0] = _kp * _error;
//    //I 积分分离
//    if ((IN_RANGE_EN_I == 0) ? 1 : (fabs(_error) < this->IN_RANGE_EN_I)) { tmp[1] = _ki * this->integral; }
//    //D 微分分离
//    if ((IN_RANGE_EN_D == 0) ? 1 : (fabs(_error) < this->IN_RANGE_EN_D)) {
//        tmp[2] = _kd * (_error - _derror);
//    }
//    //求和
//    tmp[3] = tmp[0] + tmp[1] + tmp[2] + this->ForwardLoop() * this->Kf;
//
//    /*后期处理*/
//    //输出限幅
//    tmp[3] = (tmp[3] > _max_out) ? _max_out : tmp[3];
//    tmp[3] = (tmp[3] < _min_out) ? _min_out : tmp[3];
//    //赋值
//    _derror = _error;
//    _out = tmp[3];
//    return tmp[3];
//}
//
///* 1阶惯性环节前馈环 G(s)=(1+s)/Ts */
//float cPIDPla::ForwardLoop(void) {
//    return (_ref - this->LastRef + 1) * this->Fs;
//}
//
//void cPIDPla::Reset(void) {
//    _feedback = 0;
//    this->LastRef = 0;
//    _error = 0;
//    _derror = 0;
//    this->integral = 0;
//    _out = 0;
//}

#endif