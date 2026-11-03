# ⚡ ChargeWise AI

### Intelligent Credit Card Statement Analysis & Charge Explanation

ChargeWise AI is an AI-powered financial assistant designed to help users understand their credit card statements. It analyses uploaded statement images and PDFs, extracts individual transactions, identifies potential fees and suspicious charges, and explains financial activity in simple language.

The application uses a multi-agent architecture powered by Large Language Models (LLMs), with human-in-the-loop review to ensure users can inspect and confirm flagged transactions before proceeding.

---

## 🚀 Key Features

### 1. Statement Upload & Extraction

* Upload credit card statements in PDF or image format.
* Extract transaction information using OCR and multimodal AI.
* Identify transaction dates, merchant names, amounts and descriptions.
* Convert extracted information into structured transaction records.

### 2. Intelligent Charge Classification

* Categorise transactions into meaningful groups.
* Identify potential service charges, late fees and additional fees.
* Enrich transactions using merchant information.
* Detect potentially suspicious transactions for further inspection.

### 3. AI-Powered Charge Explanation

* Explain individual transactions in plain language.
* Answer questions about specific charges.
* Summarise statement activity.
* Provide conversational assistance using extracted transaction data.

### 4. Human-in-the-Loop Review

* Automatically flag transactions requiring additional attention.
* Display flagged transactions for user review.
* Allow users to approve or reject flagged items.
* Resume the AI workflow after receiving human decisions.

### 5. Multi-Agent Architecture

ChargeWise uses specialised agents with separate responsibilities:

| Agent              | Responsibility                                  |
| ------------------ | ----------------------------------------------- |
| Orchestrator Agent | Coordinates the complete workflow               |
| Extraction Agent   | Extracts and structures transaction information |
| Explanation Agent  | Explains charges and answers user questions     |
| Human Review Agent | Handles flagged transactions and user decisions |

---

## 🏗️ Technology Stack

| Component               | Technology          |
| ----------------------- | ------------------- |
| Frontend                | React 18, Vite      |
| Styling                 | Tailwind CSS        |
| API Communication       | Axios               |
| Backend                 | Python, FastAPI     |
| Agent Orchestration     | LangGraph           |
| LLM Integration         | LangChain           |
| Local Model Runtime     | Ollama              |
| Vision Model            | LLaVA               |
| Reasoning Model         | Mistral             |
| Data Validation         | Pydantic            |
| PDF Processing          | pdfplumber          |
| OCR                     | Tesseract / EasyOCR |
| Real-Time Communication | WebSockets          |

---

## 🧠 System Architecture

```text
                 USER
                   |
                   v
            REACT FRONTEND
                   |
             File Upload / Chat
                   |
                   v
             FASTAPI BACKEND
                   |
                   v
           ORCHESTRATOR AGENT
                   |
          +--------+--------+
          |        |        |
          v        v        v
      EXTRACTION  REVIEW  EXPLANATION
        AGENT     AGENT     AGENT
          |        |        |
          v        v        |
       OCR /    HUMAN       |
       LLM      APPROVAL    |
          |        |        |
          +--------+--------+
                   |
                   v
          STRUCTURED CHARGES
                   |
                   v
          EXPLANATION RESPONSE
                   |
                   v
             REACT FRONTEND
```

### Human-in-the-Loop Workflow

1. User uploads a credit card statement.
2. Extraction Agent identifies individual transactions.
3. Orchestrator evaluates extracted transactions.
4. Suspicious or uncertain transactions are flagged.
5. LangGraph pauses the workflow for human review.
6. The frontend displays flagged transactions.
7. User approves or rejects the transactions.
8. The workflow resumes with the user's decisions.
9. Explanation Agent generates the final response.

---

## 📂 Project Structure

```text
chargeWise-ai/
│
├── backend/
│   │
│   ├── agents/
│   │   ├── orchestrator_agent.py
│   │   ├── extraction_agent.py
│   │   ├── explanation_agent.py
│   │   └── human_loop_agent.py
│   │
│   ├── tools/
│   │   ├── ocr_tool.py
│   │   ├── merchant_lookup.py
│   │   └── fee_classifier.py
│   │
│   ├── models/
│   │   └── schemas.py
│   │
│   ├── config.py
│   ├── main.py
│   └── requirements.txt
│
├── frontend/
│   │
│   ├── src/
│   │   ├── components/
│   │   │   ├── FileUpload.jsx
│   │   │   ├── ChatInterface.jsx
│   │   │   ├── ChargeCard.jsx
│   │   │   └── HumanReview.jsx
│   │   │
│   │   ├── services/
│   │   │   └── api.js
│   │   │
│   │   ├── App.jsx
│   │   └── main.jsx
│   │
│   ├── package.json
│   └── index.html
│
└── README.md
```

---

## ⚙️ Installation & Setup

### Prerequisites

Install the following:

* Python
* Node.js and npm
* Ollama
* Git
* Tesseract OCR (if using the Tesseract OCR option)

### 1. Download the AI Models

Start Ollama and download the required models:

```bash
ollama serve
```

In another terminal:

```bash
ollama pull llava
ollama pull mistral
```

### 2. Start the Backend

Navigate to the backend directory:

```bash
cd backend
```

Create a virtual environment:

```bash
python -m venv .venv
```

Activate it on Windows:

```bash
.venv\Scripts\activate
```

Install dependencies:

```bash
pip install -r requirements.txt
```

Start FastAPI:

```bash
uvicorn main:app --reload --port 8000
```

Backend API:

```text
http://localhost:8000
```

### 3. Start the Frontend

Open another terminal:

```bash
cd frontend
```

Install dependencies:

```bash
npm install
```

Start the development server:

```bash
npm run dev
```

Open the frontend using the local URL displayed by Vite.

---

## 🔌 Core Backend Responsibilities

| Component          | Description                                    |
| ------------------ | ---------------------------------------------- |
| OCR Tool           | Reads text from statement images and documents |
| Merchant Lookup    | Enriches extracted merchant information        |
| Fee Classifier     | Identifies and classifies potential fees       |
| Extraction Agent   | Produces structured transaction data           |
| Explanation Agent  | Generates natural-language explanations        |
| Human Review Agent | Processes user decisions                       |
| Orchestrator       | Coordinates agents using LangGraph             |

---

## 🔐 Human Review & Reliability

ChargeWise is designed with human oversight.

AI-generated extraction and classification results should not automatically be treated as verified financial facts.

Transactions that are uncertain or potentially suspicious can be presented for user review.

The user remains responsible for confirming transaction details and deciding whether to dispute a charge with their financial institution.

---

## 🎯 Project Objectives

* Simplify credit card statement analysis.
* Automate transaction extraction from uploaded documents.
* Identify and classify additional charges and fees.
* Provide understandable explanations of financial transactions.
* Demonstrate multi-agent AI orchestration.
* Implement human-in-the-loop decision-making.
* Integrate locally hosted LLMs using Ollama.
* Develop an interactive React-based financial assistant.

---

## 🔮 Future Enhancements

* Support additional bank statement formats.
* Improve transaction extraction accuracy.
* Integrate verified merchant and financial information sources.
* Add statement-to-statement comparison.
* Introduce spending analytics and visual dashboards.
* Improve suspicious transaction detection.
* Add secure user authentication and persistent user histories.

---

## ⚠️ Disclaimer

ChargeWise AI is an educational AI project. Its explanations and suspicious-charge indicators are informational and should not be considered professional financial advice or proof of fraud.

Users should verify extracted information against their original statements.

---

**ChargeWise AI — Understand Every Charge.**
