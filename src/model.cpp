//
//  model.cpp
//  Gravity
//
//  Created by Hijazi, Hassan (Data61, Canberra City) on 6/5/17.
//
//

#include <Gravity/model.h>
//#define USEDEBUG
#ifdef USEDEBUG
#define Debug(x) cout << x
#else
#define Debug(x)
#endif
#define DebugOn(x) cout << x
#define DebugOff(x)


using namespace std;

/** Constructor */
//@{
Model::Model(){
    _nnz_g = 0;
    _nnz_h = 0;
};
//@}

/* Destructor */
Model::~Model(){};


/* Accessors */


size_t Model::get_nb_vars() const{
    return _nb_vars;
};

size_t Model::get_nb_cons() const{
    size_t n = 0;
    for (auto &cp:_cons) {
        n+= cp.second.get_nb_instances();
    }
    return n;
};


size_t Model::get_nb_nnz_g() const{
    return _nnz_g;
};

//Split "mem" into "parts", e.g. if mem = 10 and parts = 4 you will have: 0,2,4,6,10
//if possible the function will split mem into equal chuncks, if not
//the last chunck will be slightly larger

std::vector<int> bounds(int parts, int mem) {
    std::vector<int>bnd;
    int delta = mem / parts;
    int reminder = mem % parts;
    int N1 = 0, N2 = 0;
    bnd.push_back(N1);
    for (int i = 0; i < parts; ++i) {
        N2 = N1 + delta;
        if (i == parts - 1)
            N2 += reminder;
        bnd.push_back(N2);
        N1 = N2;
    }
    return bnd;
}



size_t Model::get_nb_nnz_h() const{
    size_t idx=0;
    /* return the structure of the hessian */
    for(auto& v: _vars)
    {
//        vid = v->get_idx();
//        //        v->print();
//        //        cout << "hessian link :\n";
//        for(auto itv = v->_hess.cbegin(); itv != v->_hess.cend(); ++itv)
//        {
//            vjd = *itv;
//            if (vjd <= vid) { // This is a symmetric matrix, fill the lower left triangle only.
//                //                vj = this->_vars.find(vjd)->second;
//                //                vj->print();
//                //                cout << " , ";
//                idx++;
//            }
//        }
        //        cout << "\n";
    }
    //        cout << "Hessian nnz = " << idx << endl;
    //    exit(-1);
    return idx;
};

//

Constraint* Model::get_constraint(const string& cname) const{
    return (Constraint*)&_cons.at(cname);
}

param_* Model::get_var(const string& vname) const{
        return _vars.at(vname);
}



/* Modifiers */

void Model::add_var(const param_& v){
    if (v.is_transposed()) {
        _nb_vars += v.get_dim();
    }
    else {
        _nb_vars++;
    }
    if (_vars.count(v.get_name())!=0) {
        _vars[v.get_name()] = (param_*)copy(v);
    }
};


void Model::del_var(const param_& v){
    if (v.is_transposed()) {
        _nb_vars -= v.get_dim();
    }
    else {
        _nb_vars--;
    }
    auto it = _vars.find(v.get_name());
    delete it->second;
    _vars.erase(it);
};

void Model::add_constraint(const Constraint& c){
    _nb_cons += c.get_nb_instances();
    if (_cons.count(c.get_name())!=0) {
        _cons[c.get_name()] = c;
    }
    //UPDATE HESSIAN
    //    c->print();
};




void Model::add_on_off(const Constraint& c, var<bool>& on){
    if (c.get_ftype() != lin_) {
        cerr << "Nonlinear constraint.\n";
        exit(-1);
    }
    Constraint res(c.get_name() + "_on/off");
    double b;
//    for(auto it: orig_q->_coefs) {
//        v = getparam_<double>(it.first);
//        if (!v->is_bounded_below() || !v->is_bounded_above()) {
//            cerr << "Variable " << v->_name << " in constraint " << c._name << " does not have finite bounds.\n";
//            exit(1);
//        }
//        if (c.get_type() == leq || c.get_type() == eq) {
//            if (it.second < 0) res -= it.second*v->get_lb_off()*(1-on);
//            else res -= it.second*v->get_ub_off()*(1-on);
//        }
//        else{ // geq
//            if (it.second < 0) res -= it.second*v->get_ub_off()*(1-on);
//            else res -= it.second*v->get_lb_off()*(1-on);
//        }
//    }
//    if (c.get_type() == eq) {
//        Constraint res2(c.get_name() + "_on/off2");
//        for(auto it: orig_q->_coefs) {
//            v = getparam_<double>(it.first);
//            if (it.second < 0) res2 -= it.second*v->get_ub_off()*(1-on);
//            else res2 -= it.second*v->get_lb_off()*(1-on);
//        }
//        res2 += *orig_q;
//        res2 -= b*on;
//        res2 >= 0;
//        addConstraint(res2);
//    }
//    res += *orig_q;
//    res -= orig_q->get_const();
//    res -= b*on;
//    if (c.get_type() == eq or c.get_type() == leq) res <= 0;
//    else res >= 0;
    add_constraint(res);
}

void Model::add_on_off(var<>& v, var<bool>& on){
//    if(v.get_ub() != v.get_ub_off()) {
//        Constraint UB(v._name + "_UB_on/off");
//        UB += v - v.get_ub() * on - (1 - on) * v.get_ub_off();
//        UB <= 0;
//        addConstraint(UB);
//    }
//    if(v.get_lb() != v.get_lb_off()) {
//        Constraint LB(v._name + "_LB_on/off");
//        LB += v - v.get_lb() * on - (1 - on) * v.get_lb_off();
//        LB >= 0;
//        addConstraint(LB);
//    }
}

void Model::add_McCormick(std::string name, var<>& v, var<>& v1, var<>& v2) {
    Constraint MC1(name+"_McCormick1");
    MC1 += v;
    MC1 -= v1.get_lb()*v2 + v2.get_lb()*v1 - v1.get_lb()*v2.get_lb();
    MC1 >= 0;
    add_constraint(MC1);
    //    MC1.print();
    Constraint MC2(name+"_McCormick2");
    MC2 += v;
    MC2 -= v1.get_ub()*v2 + v2.get_ub()*v1 - v1.get_ub()*v2.get_ub();
    MC2 >= 0;
    add_constraint(MC2);
    //    MC2.print();
    Constraint MC3(name+"_McCormick3");
    MC3 += v;
    MC3 -= v1.get_lb()*v2 + v2.get_ub()*v1 - v1.get_lb()*v2.get_ub();
    MC3 <= 0;
    add_constraint(MC3);
    //    MC3.print();
    Constraint MC4(name+"_McCormick4");
    MC4 += v;
    MC4 -= v1.get_ub()*v2 + v2.get_lb()*v1 - v1.get_ub()*v2.get_lb();
    MC4 <= 0;
    add_constraint(MC4);
    //    MC4.print();
}


void Model::add_on_off_McCormick(std::string name, var<>& v, var<>& v1, var<>& v2, var<bool>& on) {
    Constraint MC1(name+"_McCormick1");
    MC1 += v;
    MC1 -= v1.get_lb()*v2 + v2.get_lb()*v1 - v1.get_lb()*v2.get_lb();
    MC1 >= 0;
    add_on_off(MC1, on);
    Constraint MC2(name+"_McCormick2");
    MC2 += v;
    MC2 -= v1.get_ub()*v2 + v2.get_ub()*v1 - v1.get_ub()*v2.get_ub();
    MC2 >= 0;
    add_on_off(MC2, on);
    Constraint MC3(name+"_McCormick3");
    MC3 += v;
    MC3 -= v1.get_lb()*v2 + v2.get_ub()*v1 - v1.get_lb()*v2.get_ub();
    MC3 <= 0;
    add_on_off(MC3, on);
    Constraint MC4(name+"_McCormick4");
    MC4 += v;
    MC4 -= v1.get_ub()*v2 + v2.get_lb()*v1 - v1.get_ub()*v2.get_lb();
    MC4 <= 0;
    add_on_off(MC4, on);
}


void Model::del_constraint(const Constraint& c){
    //    _cons.erase(c->get_idx());
    assert(false);
};

void Model::set_objective(const func_& f) {
    _obj = f;
}

void Model::set_objective_type(ObjectiveType t) {
    _objt = t;
}


void Model::check_feasible(const double* x){
    int vid = 0;
    //    param_* v = NULL;
    var<>* var = NULL;
    /* return the structure of the hessian */
//    for(auto& v: _vars)
//    {
//        vid = v->get_idx();
//        var = getparam_<double>(vid);
//        if ((x[vid] - var->get_ub())>1e-6) {
//            cerr << "violated upper bound constraint: ";
//            var->print();
//        }
//        if ((x[vid] - var->get_lb())<-1e-6) {
//            cerr << "violated lower bound constraint: ";
//            var->print();
//        }
//    }
//    int cid = 0;
//    for(auto& c: _cons)
//    {
//        cid = c->get_idx();
//        switch (c->get_type()) {
//            case eq:
//                if(fabs(c->eval(x)-c->_rhs) > 1e-6) {
//                    cerr << "violated constraint: ";
//                    c->print();
//                    printf ("violation = %.10f;\n",(c->eval(x)-c->_rhs));
//                }
//                break;
//            case leq:
//                if((c->eval(x)-c->_rhs) > 1e-6) {
//                    cerr << "violated constraint: ";
//                    c->print();
//                    printf ("violation = %.10f;\n",(c->eval(x)-c->_rhs));
//                }
//                break;
//            case geq:
//                if((c->eval(x)-c->_rhs) < -1e-6) {
//                    cerr << "violated constraint: ";
//                    c->print();
//                    printf ("violation = %.10f;\n",(c->eval(x)-c->_rhs));
//                }
//                break;
//                
//            default:
//                break;
//        }
//    }
}


void Model::fill_in_param_bounds(double* x_l ,double* x_u) {
    size_t idx = 0;
    param_* v;
    for(auto& v_p: _vars)
    {
        v = v_p.second;
        switch (v->get_intype()) {
            case float_: {
                auto real_var = (var<float>*)v;
                for (int i = 0; i < real_var->get_dim(); i++) {
                    x_l[idx] = (double)real_var->get_lb();
                    x_u[idx] = (double)real_var->get_ub();
                    idx++;
                }
                break;
            }
            case long_:{
                auto real_var = (var<long double>*)v;
                for (int i = 0; i < real_var->get_dim(); i++) {
                    x_l[idx] = (double)real_var->get_lb();
                    x_u[idx] = (double)real_var->get_ub();
                    idx++;
                }
                break;
            }
            case double_:{
                auto real_var = (var<double>*)v;
                for (int i = 0; i < real_var->get_dim(); i++) {
                    x_l[idx] = (double)real_var->get_lb();
                    x_u[idx] = (double)real_var->get_ub();
                    idx++;
                }
                break;
            }
            case integer_:{
                auto real_var = (var<int>*)v;
                for (int i = 0; i < real_var->get_dim(); i++) {
                    x_l[idx] = (double)real_var->get_lb();
                    x_u[idx] = (double)real_var->get_ub();
                    idx++;
                }
                break;
            }
            case short_:{
                auto real_var = (var<short>*)v;
                for (int i = 0; i < real_var->get_dim(); i++) {
                    x_l[idx] = (double)real_var->get_lb();
                    x_u[idx] = (double)real_var->get_ub();
                    idx++;
                }
                break;
            }
            case binary_:{
                auto real_var = (var<bool>*)v;
                for (int i = 0; i < real_var->get_dim(); i++) {
                    x_l[idx] = (double)real_var->get_lb();
                    x_u[idx] = (double)real_var->get_ub();
                    idx++;
                }
                break;
            }
            default:
                break;
        } ;
        idx++;
    }
    //    cout << "idx = " << idx << endl;
}

void Model::fill_in_obj(const double* x , double& res){
    res = _obj.eval(x);
}

void Model::fill_in_cstr(const double* x , double* res, bool new_x){
    int idx=0;
    
    for(auto& c: _cons)
    {
//        res[idx] = c->eval(x);
        idx++;
    }
}

void Model::fill_in_jac(const double* x , double* res, bool new_x){
    int idx=0;
    int cid = 0;
    int vid = 0;
    //    Constraint* c = NULL;
    param_* v = NULL;
    func_* dfdx = NULL;
    int meta_link = -1;

    /* return the values of the jacobian of the constraints */
    for(auto& c :_cons)
    {
//        for(auto itv = c->_vars->cbegin(); itv != c->_vars->cend(); ++itv)
//        {
//            vid = itv->first;
//            v = itv->second;
//                res[idx] = c->eval_dfdx(vid, x);
            //                res[idx] = c->eval_dfdx(vid, x);
//            idx++;
//        }
    }
}


void Model::fill_in_jac_nnz(int* iRow , int* jCol){
    int idx=0;
    int cid = 0;
    int vid = 0;
    //    Constraint* c = NULL;
    param_* v = NULL;
    /* return the structure of the jacobian */
    for(auto& c :_cons)
    {
//        cid = c->get_idx();
//        assert(cid==c->get_idx());
//        for(auto itv = c->_vars.cbegin(); itv != c->_vars.cend(); ++itv)
//        {
//            vid = itv->first;
//            v = itv->second;
//            assert(vid==v->get_idx());
//            iRow[idx] = cid;
//            jCol[idx] = vid;
//            idx++;
//        }
    }
}

void Model::fill_in_hess_nnz(int* iRow , int* jCol){
    int idx=0;
    int vid = 0, vjd = 0;
    //    param_* v = NULL;
    /* return the structure of the hessian */
    for(auto& v: _vars)
    {
//        vid = v->get_idx();
//        for(auto itv = v->_hess.cbegin(); itv != v->_hess.cend(); ++itv)
//        {
//            vjd = *itv;
//            if (vjd <= vid) { // This is a symmetric matrix, fill the lower left triangle only.
//                iRow[idx] = vid;
//                jCol[idx] = vjd;
//                v->_hess_id.insert(pair<int, int>(vjd, idx));
//                getVar(vjd)->_hess_id.insert(pair<int, int>(vid, idx));
//                //                _hess_index.insert(pair<string, int>(to_string(vid)+','+to_string(vjd), idx));
//                idx++;
//            }
//        }
    }
}

#ifdef USE_IPOPT
void Model::fill_in_param_linearity(Ipopt::TNLP::LinearityType* param_types){
    int vid = 0, cid = 0;
    Constraint* c = NULL;
    bool linear = true;
    for(auto& vi: _vars)
    {
//        vid = vi->get_idx();
//        linear = true;
//        for(auto itc = vi->_cstrs.cbegin(); itc != vi->_cstrs.cend(); ++itc)
//        {
//            cid = itc->first;
//            c = itc->second;
//            if (c->get_ftype()>lin_) {
//                linear=false;
//            }
//        }
        if (linear) param_types[vid]=Ipopt::TNLP::LINEAR;
        else param_types[vid] = Ipopt::TNLP::NON_LINEAR;
    }
}


void Model::fill_in_cstr_linearity(Ipopt::TNLP::LinearityType* const_types){
    int cid = 0;
    for(auto& c :_cons)
    {
//        cid = c->get_idx();
//        if (c->get_ftype()<=lin_) {
//            const_types[cid]=Ipopt::TNLP::LINEAR;
//        }
//        else {
//            const_types[cid] = Ipopt::TNLP::NON_LINEAR;
//        }
    }
}
#endif


void Model::fill_in_hess(const double* x , double obj_factor, const double* lambda, double* res, bool new_x){
    int vid = 0, vjd = 0, cid = 0, idx = 0;
    param_* vi = NULL;
    func_* obj_dfdx = NULL;
    func_* dfdx = NULL;
    double hess = 0;
    for(auto& v: _vars)
    {
//        vid = v->get_idx();
//        
//        for(auto itv = v->_hess.cbegin(); itv != v->_hess.cend(); ++itv)
//        {
//            vjd = *itv;
//            if (vjd <= vid) { // This is a symmetric matrix, fill the lower left triangle only.
//                res[idx] = 0;
//                idx++;
//            }
//        }
    }
    
//    for (auto &it1:_obj->_vars){
//        vid = it1.first;
//        vi = it1.second;
//        if (_obj->get_type()==nlin_) {
//            //            obj_dfdx = _obj->get_dfdx(vi);
//            //            if (obj_dfdx && !_obj->has_dfdx(vid)) {
//            //                _obj->set_dfdx(vid, obj_dfdx);
//            //            }
//            //        }
//            _obj->compute_dfdx(vi);
//        }
//        for (auto &it2:_obj->_vars){
//            vjd = it2.first;
//            if (vjd<=vid) {
//                if(_obj->_hess.count(vid)==1 && _obj->_hess.count(vjd)==1) {
//                    if(_obj->get_type()==nlin_ && obj_dfdx){
//                        if(obj_dfdx->has_var(vjd))
//                            hess = obj_dfdx->eval_dfdx(vjd, x);
//                        else
//                            hess = 0;
//                    }
//                    else{
//                        hess = _obj->get_q_coeff(vid, vjd);
//                        if (vid==vjd)
//                            hess *= 2;
//                    }
//                    if(hess != 0)
//                        res[get_hess_index(vi, vjd)] = obj_factor * hess;
//                }
//                else
//                    res[get_hess_index(vi, vjd)] = 0;
//                //                else
//                //                    res[get_hess_index(vid, vjd)] = 0;
//            }
//        }
//    }

    for (auto& c:_cons) {
        
//        for (auto it1:c->_vars){
//            vid = it1.first;
//            if(c->_hess.count(vid)==0)
//                continue;
//            vi = it1.second;
//            for (auto& it2:*c->_hess[vid]){
//                vjd = it2;
//                hess = c->get_q_coeff(vid, vjd);
//                
//                if (vid==vjd) {
//                        hess *= 2;
//                }
//                if(hess != 0)
//                    res[get_hess_index(vi, vjd)] += lambda[cid] * hess;
//            }
//        }
    }
}






void Model::fill_in_grad_obj(const double* x , double* res){
    int idx=0;
    //    param_* v = NULL;
    int vid = 0;
    for(auto& vi: _vars)
    {
//        vid = vi->get_idx();
//        
//        if (!_obj->has_var(vid)) {
//            res[idx] = 0;
//            idx++;
//            continue;
//        }
//        res[idx] = _obj->eval_dfdx(vid, x);
        idx++;
    }
}


void Model::fill_in_param_init(double* x) {
    var<int>* int_var = NULL;
    var<bool>* bin_var = NULL;
    var<float>* real_var = NULL;
    var<>* long_real_var = NULL;
    int idx=0;
    int vid = 0;
    for(auto& vi: _vars)
    {
//        vid = vi->get_idx();
//        
//        switch (vi->get_type()) {
//            case real:
//                real_var = (var<float>*)vi;
//                x[idx] = (double)real_var->get_value();
//                break;
//            case longreal:
//                long_real_var = (var<>*)vi;
//                x[idx] = long_real_var->get_value();
//                break;
//            case integ:
//                int_var = (var<int>*)vi;
//                x[idx] = (double)int_var->get_value();
//                break;
//            case binary:
//                bin_var = (var<bool>*)vi;
//                x[idx] = (double)bin_var->get_value();
//                break;
//            default:
//                exit(-1);
//                break;
//        } ;
        idx++;
    }
}

void Model::fill_in_cstr_bounds(double* g_l ,double* g_u) {
    int idx=0;
    int cid = 0;
    //    Constraint* c = NULL;
    for(auto& c : _cons)
    {
//        cid = c->get_idx();
//        switch (c->get_type()) {
//            case eq:
//                g_l[idx] = c->_rhs;
//                g_u[idx] = c->_rhs;
//                //                g_l[idx] = 0;
//                //                g_u[idx] = 0;
//                break;
//            case leq:
//                g_l[idx] = -INFINITY;
//                g_u[idx] = c->_rhs;
//                //                g_u[idx] = -c->get_const();
//                //                cout << "gl[idx] = " <<g_l[idx];
//                //                cout << ", gu[idx] = " <<g_u[idx] << endl;
//                break;
//            case geq:
//                g_l[idx] = c->_rhs;
//                //                g_l[idx] = -c->get_const();
//                g_u[idx] = INFINITY;
//                break;                
//                
//            default:
//                exit(-1);
//                break;
//        }
        idx++;
    }
}

#ifdef USE_BONMIN
void Model::fill_in_param_types(Bonmin::TMINLP::VariableType* param_types){
    int vid = 0;
    for(auto& vi: _vars){
//        vid = vi->get_idx();
//        switch (vi->get_type()) {
//            case real:
//                param_types[vid] = Bonmin::TMINLP::CONTINUOUS;
//                break;
//            case longreal:
//                param_types[vid] = Bonmin::TMINLP::CONTINUOUS;
//                break;
//            case integ:
//                param_types[vid] = Bonmin::TMINLP::INTEGER;
//                break;
//            case binary:
//                param_types[vid] = Bonmin::TMINLP::BINARY;
//                break;
//            default:
//                break;
//        } ;
    }
}

#endif

void Model::print_functions() const{
    cout << "Number of atomic functions = " << _functions.size();
    cout << endl;
    //    for (auto& f: _functions){
    //        f->print(false);
    //        cout << endl;
    //    }
    cout << endl;
}

void Model::print_solution() const{
    var<int>* int_var = NULL;
    var<bool>* bin_var = NULL;
    var<float>* real_var = NULL;
    var<double>* long_real_var = NULL;
    int idx=0;
    int vid = 0;
    for(auto& v: _vars ) {
//        vid = v->get_idx();
//        //        assert(vid==idx);
//        switch (v->get_type()) {
//            case real:
//                real_var = (var<float>*)v;
//                real_var->print();
//                break;
//            case longreal:
//                long_real_var = (var<double>*)v;
//                long_real_var->print();
//                break;
//            case integ:
//                int_var = (var<int>*)v;
//                int_var->print();
//                break;
//            case binary:
//                bin_var = (var<bool>*)v;
//                bin_var->print();
//                break;
//            default:
//                break;
//        } ;
        idx++;
    }
}

void Model::print_constraints() const{
    for(auto& p: _cons){
        p.second.print();
    }
}