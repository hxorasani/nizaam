#! /usr/bin/python

# Python tool to convert a 3D model from the text obj format to JS arrays

import sys

def printHelp():
  print("Convert 3D model in OBJ format (text, triangulated) to JS array")
  print("usage:\n")
  print("  python obj2array.py [-c -sX -uY -vZ -n] file\n")
  print("  -c     compact format (off by default)")
  print("  -t     use direct instead of indexed UV coords (off by default)")
  print("  -m     include a material array (per-triangle)")
  print("  -nS    use the name S for the model (defaut: \"model\")")
  print("  -sX    scale the model by X (default: 1)")
  print("  -uY    scale the U texture coord by Y (default: 1)")
  print("  -vZ    scale the V texture coord by Z (default: 1)")
  print("");

if len(sys.argv) < 2:
  printHelp()
  quit()

FILENAME = ""
VERTEX_SCALE = 5
U_SCALE = 5
V_SCALE = 5
NAME = ""
COMPACT = False
INDEXED_UVS = True
MATERIALS = False
SHORT_TYPE = ""
FLOAT_TYPE = ""

for s in sys.argv:
  if s == "-c":
    COMPACT = True
  elif s == "-T":
    SHORT_TYPE = "new Uint16Array"
    FLOAT_TYPE = "new Float32Array"
  elif s == "-t":
    INDEXED_UVS = False
  elif s == "-m":
    MATERIALS = True
  elif s[:2] == "-s":
    VERTEX_SCALE = int(s[2:])
  elif s[:2] == "-u":
    U_SCALE = int(s[2:])
  elif s[:2] == "-v":
    V_SCALE = int(s[2:])
  elif s[:2] == "-n":
    NAME = s[2:]
  else:
    FILENAME = s
    if s[-3:] != ".py": NAME = s[:-4]

objFile = open(FILENAME)

vertices = []
uvs = []
triangles = []
triangleUVs = []
materials = []
materialNames = []

currentMatrial = 0

def getMaterialIndex(materialName):
  try:
    return materialNames.index(materialName)
  except Exception:
    materialNames.append(materialName)
    return len(materialNames) - 1

# parse the file:

for line in objFile:
  if line[:2] == "v ":
    coords = line[2:].split()
    vertex = [float(coords[i]) for i in range(3)]
    vertex[2] *= -1
    vertices.append(vertex)
  elif line[:3] == "vt ":
    coords = line[3:].split()
    vertex = [float(coords[i]) for i in range(2)]
    vertex[1] = 1.0 - vertex[1]
    uvs.append(vertex)
  elif line[:2] == "f ":
    indices = line[2:].split()

    if len(indices) != 3:
      raise(Exception("The model is not triangulated!"))

    t = []
    u = []
 
    for i in indices:
      components = i.split("/")
      t.append(int(components[0]) - 1)
      try:
        u.append(int(components[1]) - 1)
      except Exception as e:
        u.append(int(components[2]) - 1)

    triangles.append(t)
    triangleUVs.append(u)
    materials.append([currentMatrial])
  elif line[:7] == "usemtl ":
    currentMatrial = getMaterialIndex(line[7:])

# print the result:

def arrayString(name, array, components, scales, align, short, dataType):
  result = "const " + name + " = " + dataType + "([\n"

  if COMPACT:
    lineLen = 0
    first = True
    n = 0

    for v in array:
      for c in v:
        item = ""

        if first:
          first = False
        else:
          result += ","
          lineLen += 1

          if lineLen >= 80:
            result += "\n"
            lineLen = 0

        num = c * scales[n % len(scales)]

        if short:
          item += str(num)
        else:
          item += ("" if num >= 0 else "-") + "0x%x" % abs(num)

        if lineLen + len(item) >= 80:
          result += "\n"
          lineLen = 0
       
        result += item
        lineLen += len(item)
        n += 1

    result += "]);\n"

  else: # non-compact
    n = 0
    endIndex = len(array) - 1

    for v in array:
      line = "  " + ", ".join([str(int(v[c] * scales[c % len(scales)])).rjust(align) for c in range(components)])

      if n < endIndex:
        line += ","

      line = line.ljust((components + 2) * (align + 1)) + "// " + str(n * components) + "\n"
      result += line
      n += 1

    result += "]); // " + name + "\n"

  return result

result = ""



print("var " + NAME + "Model;")
print(";(function(){")

print(arrayString(NAME + "Vertices",vertices,3,[VERTEX_SCALE],5,False,FLOAT_TYPE))

print(arrayString(NAME + "TriangleIndices",triangles,3,[1],5,True,SHORT_TYPE))

if MATERIALS:
  print(arrayString(NAME + "Materials",materials,1,[1],5,True,SHORT_TYPE))

uvs2 = []
if INDEXED_UVS:
  print(arrayString(NAME + "UVs",uvs,2,[U_SCALE,V_SCALE],5,False,FLOAT_TYPE))
  print(arrayString(NAME + "UVIndices",triangleUVs,3,[1],5,True,SHORT_TYPE))
else:
  for item in triangleUVs:
    uvs2.append([
      uvs[item[0]][0],
      uvs[item[0]][1],
      uvs[item[1]][0],
      uvs[item[1]][1],
      uvs[item[2]][0],
      uvs[item[2]][1]])

  print(arrayString(NAME + "DirectUVs",uvs2,6,[U_SCALE,V_SCALE],5,False,FLOAT_TYPE))

print(NAME + "Model = function() {")
print("	return {")
print("		tris		: " + NAME + "Vertices,")
print("		verts		: " + NAME + "TriangleIndices,")
print("		verts_count	: " + str(len(vertices))  + "," )
print("		tris_count	: " + str(len(triangles)) + "," )
if INDEXED_UVS:
  print("		uv		: " + NAME + "UVs,")
  print("		uv_indices	: " + NAME + "UVIndices,")
  print("		uv_count	: " + str(len(uvs)) + "," )
  print("		uv_index_count	: " + str(len(triangleUVs)) + "," )
else:
  print("		direct_uv		: " + NAME + "DirectUVs,")
  print("		direct_uv_count	: " + str(len(uvs2)) + "," )
print("	};")
print("};")

print("})();")
