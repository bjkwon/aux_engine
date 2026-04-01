# `fget` — Fetch bytes from a path or URL

## Purpose

Reads **raw bytes** from a **local file path** or **remote URL** into an auxe **byte** object (binary buffer). Use this when you need the file contents as-is (not decoded as WAV/audio or parsed as text like `file()`).

**Implementation:** `src/func/_file.cpp` (`_fget`)

---

## Syntax

```text
b = fget(source)
```

| Operand | Role |
|---------|------|
| `source` | String: filesystem path, `http://` / `https://` URL, `s3://` URI, or **`bucket/key`** shorthand for S3 (see [Source forms](#source-forms)). |

The builtin is registered as **`fget`** (`EngineRuntime::InitBuiltInFunctions` in `src/engine/AuxFunc.cpp`). The engine gate function is **`_fget`**.

---

## Return value

- A **byte-typed** signal: `bufBlockSize` is **1**, length equals the number of bytes read.
- Contents are a straight binary copy (no line-ending or text conversion).

---

## Source forms

### Local file

If `source` is not treated as a remote URL, the implementation opens it with `fopen(..., "rb")` and reads the entire file.

### HTTP / HTTPS

If `source` starts with `http://` or `https://` (case-insensitive):

- **Non-Windows:** Tries **`curl`**, then **`wget`**, to download into a temp file, then reads bytes from that file.
- **Windows:** Uses **PowerShell** `Invoke-WebRequest` to download to a temp file, then reads it.

### S3 (`s3://...`)

If `source` starts with `s3://`:

- Runs **`aws s3 cp`** to copy the object to a temp file, then reads bytes.
- **Non-Windows:** Prepends a PATH that includes Homebrew and common AWS CLI locations; can also try explicit `aws` paths if `PATH` fails.

### `bucket/key` shorthand

If opening `source` as a local file **fails**, the code may interpret `source` as **`bucket/key`** (no scheme, not an absolute path, contains a `/`, etc.—see `looks_like_s3_bucket_key` in `_file.cpp`). It then fetches `s3://bucket/key` using the same **`aws s3 cp`** path as above.

---

## Prerequisites (remote)

| Kind | Requirement |
|------|----------------|
| HTTP(S) | `curl` or `wget` on Unix; PowerShell on Windows. |
| S3 | AWS CLI configured; `aws s3 cp` must succeed for the object. |

Temp files: non-Windows uses `/tmp/aux2_remote_XXXXXX`; Windows uses `tmpnam` with a `.tmp` suffix. Temp files are removed after reading.

---

## Errors

Failures from `fetch_bytes_from_source` are thrown as auxe exceptions, including:

- Missing/unopenable local file (`File not found or cannot be opened: …`).
- Seek/size/read errors on a local file.
- Download failures (curl/wget/PowerShell or `aws s3 cp`).
- Unsupported remote scheme (if `is_remote_url` is false but path logic still fails).
- Temp file creation failure.

---

## Relationship to other file builtins

| Builtin | Typical use |
|---------|-------------|
| **`fget`** | Raw bytes (byte object). |
| **`file()`** | Paths: may load WAV as audio, or text/numbers as cell/vectors (see `_file` / `filetype`). |
| **`wave()`** | WAV audio with optional time range. |

---

## Example

```text
b = fget("https://example.com/data.bin")
% b is byte type; length = size in bytes

local = fget("/path/to/file.dat")
```

For S3:

```text
b = fget("s3://my-bucket/path/to/object.bin")
% or, if local open fails and string matches bucket/key rules:
b = fget("my-bucket/path/to/object.bin")
```

---

## Registration

- **Name:** `fget`
- **Gate:** `_fget`
- **Descriptor:** `alwaysstatic = true`; required arg described as `"source"` (string).
