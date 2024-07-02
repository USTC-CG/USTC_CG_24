//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef USDFOAM_TOKENS_H
#define USDFOAM_TOKENS_H

/// \file usdFoam/tokens.h

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// 
// This is an automatically generated file (by usdGenSchema.py).
// Do not hand-edit!
// 
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

#include "pxr/pxr.h"
#include "pxr/usd/usdFoam/api.h"
#include "pxr/base/tf/staticData.h"
#include "pxr/base/tf/token.h"
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE


/// \class UsdFoamTokensType
///
/// \link UsdFoamTokens \endlink provides static, efficient
/// \link TfToken TfTokens\endlink for use in all public USD API.
///
/// These tokens are auto-generated from the module's schema, representing
/// property names, for when you need to fetch an attribute or relationship
/// directly by name, e.g. UsdPrim::GetAttribute(), in the most efficient
/// manner, and allow the compiler to verify that you spelled the name
/// correctly.
///
/// UsdFoamTokens also contains all of the \em allowedTokens values
/// declared for schema builtin attributes of 'token' scene description type.
/// Use UsdFoamTokens like so:
///
/// \code
///     gprim.GetMyTokenValuedAttr().Set(UsdFoamTokens->edgeIndices);
/// \endcode
struct UsdFoamTokensType {
    USDFOAM_API UsdFoamTokensType();
    /// \brief "edgeIndices"
    /// 
    /// UsdFoamPlateauBorder
    const TfToken edgeIndices;
    /// \brief "plateauBorder"
    /// 
    /// A special structure in the foam system.
    const TfToken plateauBorder;
    /// \brief "points"
    /// 
    /// UsdFoamPlateauBorder
    const TfToken points;
    /// \brief "polygonFilm"
    /// 
    /// A film represented by polygons intersected by spheres.
    const TfToken polygonFilm;
    /// \brief "polygonIndices"
    /// 
    /// UsdFoamSphereFilm
    const TfToken polygonIndices;
    /// \brief "polygonPoints"
    /// 
    /// UsdFoamSphereFilm
    const TfToken polygonPoints;
    /// \brief "radii"
    /// 
    /// UsdFoamPlateauBorder
    const TfToken radii;
    /// \brief "sphereCenters"
    /// 
    /// UsdFoamPolygonFilm, UsdFoamSphereFilm
    const TfToken sphereCenters;
    /// \brief "sphereFilm"
    /// 
    /// A film represented by spheres intersected by polygons.
    const TfToken sphereFilm;
    /// \brief "sphereRadii"
    /// 
    /// UsdFoamPolygonFilm, UsdFoamSphereFilm
    const TfToken sphereRadii;
    /// \brief "FoamRoot"
    /// 
    /// Schema identifer and family for UsdFoamRoot
    const TfToken FoamRoot;
    /// \brief "PlateauBorder"
    /// 
    /// Schema identifer and family for UsdFoamPlateauBorder
    const TfToken PlateauBorder;
    /// \brief "PolygonFilm"
    /// 
    /// Schema identifer and family for UsdFoamPolygonFilm
    const TfToken PolygonFilm;
    /// \brief "SphereFilm"
    /// 
    /// Schema identifer and family for UsdFoamSphereFilm
    const TfToken SphereFilm;
    /// A vector of all of the tokens listed above.
    const std::vector<TfToken> allTokens;
};

/// \var UsdFoamTokens
///
/// A global variable with static, efficient \link TfToken TfTokens\endlink
/// for use in all public USD API.  \sa UsdFoamTokensType
extern USDFOAM_API TfStaticData<UsdFoamTokensType> UsdFoamTokens;

PXR_NAMESPACE_CLOSE_SCOPE

#endif
