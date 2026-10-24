ID:              T029
Title:           Final self-review and rubric traceability update
Milestone:       M7
Tier:            MUST
Depends on:      [T024, T025, T027, T028]
Blocks:          []
Owner:           (unassigned)
Estimate:        2 / 3 / 5 hours
Files to create or modify:   PLAN.md; docs/rubric-traceability.md; docs/assumptions.md; docs/milestones.md; docs/risk-register.md
Description:     Run every checklist item, replace generic rubric rows when the official rubric exists, record actual command results, and document all cuts or open questions.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is the final repository state and assignment text. Output is an evidence-backed release checklist.
Acceptance criteria:   1. Every checklist item has a checked result or explicit exception. 2. Every rubric line maps to file, test, and demo step. 3. No unverified claim remains. 4. Release archive hash is recorded.
Tests to write first: names and what they assert: `self_review_paths`; `rubric_rows_complete`; assert referenced files exist and required commands are listed.
Edge cases to cover: late rubric addition; date change; dropped SHOULD feature; platform-specific sanitizer omission; failed clean-machine run.
Out of scope:          changing code solely to improve a report; adding unplanned features after freeze.
Definition of done:    code merged, tests green, docs updated, dump flag works
