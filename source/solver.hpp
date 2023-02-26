#include <functional>
#include <iostream>
#include <Eigen/Dense>
#include <cmath>
#include <boost/math/quadrature/gauss_kronrod.hpp>

using namespace boost::math::quadrature;
using BaseFunctions = std::vector<std::function<double(double)>>;


BaseFunctions getElementBaseInRange(std::pair<double, double> &elemRange) {
    double xBegin = elemRange.first;
    double xEnd = elemRange.second;

    auto base1 = [xBegin, xEnd](double x) {
        return (x - xBegin) / (xEnd - xBegin);
    };

    auto base2 = [xBegin, xEnd](double x) {
        return (xEnd - x) / (xEnd - xBegin);
    };

    return {base2, base1};
}

BaseFunctions getElementBasesDerivative(std::pair<double, double> &elemRange) {
    double xBegin = elemRange.first;
    double xEnd = elemRange.second;

    auto baseDeriv1 = [xBegin, xEnd](double x) {
        return 1 / (xEnd - xBegin);
    };

    auto baseDeriv2 = [xBegin, xEnd](double x) {
        return -1 / (xEnd - xBegin);
    };

    return {baseDeriv2, baseDeriv1};
}

std::pair<std::vector<double>, std::vector<double>> femAcusticVibrations(int n) {
    double error;
    double Q = 0.0;

    std::pair<double, double> range{0.0, 2.0};
    double step = (range.second - range.first) / n;

    int basesNum = n + 1;

    Eigen::MatrixXd B(basesNum, basesNum); 
    B.setZero();

    Eigen::VectorXd A(basesNum);
    A.setZero();

    for(int elem = 0; elem < n; elem++) {
        std::pair<double, double> elemRange{step * elem, step * (elem + 1)};
        auto bases = getElementBaseInRange(elemRange);
        auto basesDeriv = getElementBasesDerivative(elemRange);

        for(int i = elem; i < elem + bases.size(); i++){
            for(int j = elem; j < elem + bases.size(); j++){
                auto funUnderInt = [&bases, &basesDeriv, i, j, elem](double x){
                    return basesDeriv[i - elem](x) * basesDeriv[j - elem](x) - 
                    bases[i - elem](x) * bases[j - elem](x);
                };
                Q = gauss_kronrod<double, 15>::integrate(funUnderInt, elemRange.first, elemRange.second, 0, 0, &error);
                B(i, j) += Q;
                if(elem == n - 1){
                    B(i, j) -= basesDeriv[j - elem](2) * bases[i - elem](2);
                }

                auto resFun = [&bases, &basesDeriv, i, j, elem](double x){
                    return bases[i - elem](x) * sin(x);
                };
                Q = gauss_kronrod<double, 15>::integrate(resFun, elemRange.first, elemRange.second, 0, 0, &error);
                A(i) += Q;
            }
        }
    }

    for(int i = 0; i < basesNum; i++){
        B(0, i) = 0;
    }
    B(0, 0) = 1;
    A(0) = 0;

    Eigen::VectorXd res = B.colPivHouseholderQr().solve(A);

    std::vector<double> xs;
    std::vector<double> ys;

    int pointsInElem = 1000 / n;
    int begin = 0;

    for(int elem = 0; elem < n; elem++) {
        std::pair<double, double> elemRange{step * elem, step * (elem + 1)};
        double s = (elemRange.second - elemRange.first) / pointsInElem;
        auto bases = getElementBaseInRange(elemRange);

        for (int i = begin; i < begin + pointsInElem; i++) {
            xs.push_back(elemRange.first + (i - begin) * s);
            double y = 0.0;
            for(int j = elem; j < elem + bases.size(); j++){
                y += (res(j) * bases[j - elem](xs.back()));
            }
            ys.push_back(y);
        }
        begin += pointsInElem;
    }

    return {xs, ys};
}  
 
