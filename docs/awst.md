# `awst` — Amazon Transcribe via S3

## Purpose

Uploads an **audio** signal to **Amazon S3**, starts an **Amazon Transcribe** batch job on that object, and returns a **struct** with the API’s `TranscriptionJob` payload (or a minimal fallback if JSON parsing fails).

**Prerequisites:** The `aws` CLI must be installed, configured with valid credentials, and able to run `aws s3 cp` and `aws transcribe start-transcription-job` for the chosen account/region. On macOS, the implementation prepends a typical `PATH` (`/opt/homebrew/bin`, `/usr/local/bin`, etc.) so GUI-launched apps can find `aws`.

**Implementation:** `src/func/_awscli.cpp`

---

## Syntax

```text
result = awst(audio, v)
```

| Argument | Role |
|----------|------|
| `audio` | First operand: an **audio** variable (the signal in `Sig` in the usual `awst(audio, v)` call form). |
| `v` | Second argument: a **struct** of options (see below). |

---

## Parameter struct `v`

| Field | Type | Default | Meaning |
|--------|------|---------|---------|
| `bucket` | string | *(required)* | S3 bucket for the uploaded WAV and for Transcribe output (`--output-bucket-name`). |
| `region` | string | `"us-east-1"` | AWS region for `start-transcription-job`. |
| `outputkey` | string | `"temp/transcripts/"` | S3 key **prefix** for the transcript JSON. A trailing `/` is appended if missing. Final key: `outputkey + jobname + ".json"`. |
| `basename` | string | `"auxlab"` | Prefix for the transcription **job name** and related object names, unless overridden by `id`. |
| `id` | string | — | If present (string), **replaces `basename`** as the job-name prefix. Values longer than 32 characters are **truncated to 32**. |
| `langcode` | cell of strings | *(empty)* | Language handling; see [Language modes](#language-modes-vlangcode). |

---

## Language modes (`v.langcode`)

| `langcode` | Behavior |
|------------|----------|
| Empty `{}` | `--identify-language` (automatic single-language identification). |
| One code, e.g. `{"en-US"}` | `--language-code` set to that code. |
| Two or more | `--identify-multiple-languages` with `--language-options` built from the list (`LanguageCode=...` entries, comma-separated). |

---

## Fixed Transcribe settings

The started job always uses:

- **Media format:** `wav`
- **`ChannelIdentification=true`** in job settings (stereo channel identification)

---

## Behavior (pipeline)

1. Validate: first argument is audio, second is a struct; `v.bucket` must be non-empty.
2. Write the signal to a **temporary 16-bit PCM WAV** (mono or stereo), sample rate from `GetFs()`.
3. Run **`aws s3 cp`** to `s3://bucket/<jobname>.wav`, where  
   `jobname = <basename-or-id-prefix>_<MM-DD_HH_MM_SS>` (local time).
4. Remove the temp file after upload (or on failure after the file was created).
5. Run **`aws transcribe start-transcription-job`** with the media URI, output bucket/key, region, and language options; capture **stdout** JSON.
6. Return a struct: if JSON contains `TranscriptionJob`, map that object into the result (strings, numbers, booleans, nested objects). Otherwise set a minimal struct with `TranscriptionJobName` and `TranscriptionJobStatus` (`"UNKNOWN"`).

If JSON parsing throws, the result still includes `TranscriptionJobName` (the computed `jobname`), `TranscriptionJobStatus` set to `"UNKNOWN"`, and `_parse_error` with the exception message.

---

## Return value

- **Normal case:** Struct whose fields mirror **`TranscriptionJob`** from the AWS CLI response (e.g. `TranscriptionJobName`, `TranscriptionJobStatus`, `Media`, `MediaFormat`, `LanguageCode`, timestamps—whatever the API returns).
- **Fallback:** At least `TranscriptionJobName`, `TranscriptionJobStatus`, and optionally `_parse_error`.

Example fields after a successful start:

- `job.TranscriptionJobName`
- `job.TranscriptionJobStatus` (e.g. `IN_PROGRESS`)
- `job.LanguageCode`
- `job.MediaFormat`
- `job.Media` (nested struct, e.g. `job.Media.MediaFileUri` → `s3://bucket/....wav`)
- `job.StartTime`, `job.CreationTime`, …

---

## Errors

Typical failures are raised as auxe exceptions:

- First argument not audio, or second argument not a struct.
- Missing or empty `bucket`.
- Temp file creation or WAV write failure.
- Non-zero exit from `aws s3 cp` or `aws transcribe start-transcription-job` (message includes `aws command failed (exit …)`).

---

## Platform notes

- **Non-Windows:** Temp file: `/tmp/aux2_awst_XXXXXX.wav` via `mkstemp`.
- **Windows:** Temporary path from `tmpnam` with a `.wav` suffix.

---

## Example

```text
v.region = "us-east-1";
v.bucket = "my-bucket";
v.langcode = {"en-US"};   % or {} for auto; or {"en-US","es-US"} for multi
job = awst(audio_signal, v)
```

Inspect `job.TranscriptionJobName`, `job.TranscriptionJobStatus`, `job.Media.MediaFileUri`, etc.

---

## Registration

Builtin name: `awst` — registered in `EngineRuntime::InitBuiltInFunctions()` (`src/engine/AuxFunc.cpp`).
